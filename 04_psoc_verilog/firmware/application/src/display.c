/************************************************************************/
/*																		*/
/*	display_ctrl.c	--	Digilent Display Controller Driver				*/
/*																		*/
/************************************************************************/
/*	Author: Sam Bobrowicz												*/
/*	Copyright 2014, Digilent Inc.										*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*		This module provides an easy to use API for controlling the 	*/
/*		Digilent display controller core (axi_dispctrl). It supports 	*/
/*		run-time resolution setting and seamless framebuffer-swapping 	*/
/*		for tear-free animation. 										*/
/*																		*/
/*		To use this driver, you must have an axi_dispctrl and axi_vdma	*/
/*		core present in your system. For information on how to properly */
/*		configure these cores within your design, refer to the 			*/
/*		axi_dispctrl data sheet accessible from Vivado and XPS.			*/
/*																		*/
/*		The following steps should be followed to use this driver:		*/
/*		1) Create a DisplayCtrl object and pass a pointer to it to 		*/
/*		   DisplayInitialize.											*/
/*		2) Call DisplaySetMode to set the desired mode					*/
/*		3) Call DisplayStart to begin outputting data to the display	*/
/*		4) To create a seamless animation, draw the next image to a		*/
/*		   framebuffer currently not being displayed. Then call 		*/
/*		   DisplayChangeFrame to begin displaying that frame.			*/
/*		   Repeat as needed, only ever modifying inactive frames.		*/
/*		5) To change the resolution, call DisplaySetMode, followed by	*/
/*		   DisplayStart again.											*/
/*																		*/
/*		This module contains code from the Xilinx Demo titled			*/
/*		"xiicps_polled_master_example.c." Xilinx XAPP888 was also 		*/
/*		referenced for information on reconfiguring the MMCM or PLL.	*/
/*		Note that functions beginning with "DisplayClk" are used 		*/
/*		internally for this purpose and should not need to be called 	*/
/*		externally.														*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		2/20/2014(SamB): Created										*/
/*																		*/
/************************************************************************/
/*
 * TODO: Functionality needs to be added to take advantage of the MMCM's fractional
 * 		 divide. This will allow a far greater number of frequencies to be synthesized.
 */


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "display.h"
#include "xil_io.h"
#include "math.h"
#include "xil_types.h"
#include "vga_modes.h"
#include "xaxivdma.h"
#include <stdio.h>

/* ------------------------------------------------------------ */
/*				Function implementations						*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */

/***	DisplayStart(DisplayCtrl *dispPtr)
**
**	Parameters:
**		dispPtr - Pointer to the initialized DisplayCtrl struct
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Starts the display.
**
*/
int StartDisplay(XAxiVdma *vdma, DisplayCtrl *dispPtr, u32 *framePtr)
{
	u32 vgaReg[5];
	ClkConfig clkReg;
	ClkMode clkMode;
	int i;
	double pxlClkFreq;


	/*
	 * Perform the VDMA driver calls required to start a transfer. Note that no data is actually
	 * transferred until the disp_ctrl core signals the VDMA core by pulsing fsync.
	 */
	XAxiVdma_DmaConfig(vdma, XAXIVDMA_READ, &(dispPtr->vdmaConfig));
	XAxiVdma_DmaSetBufferAddr(vdma, XAXIVDMA_READ, dispPtr->vdmaConfig.FrameStoreStartAddr);
	XAxiVdma_DmaStart(vdma, XAXIVDMA_READ);

	/*
	 * Configure the disp_ctrl core with the display mode timing parameters
	 */
	vgaReg[0] = (dispPtr->vMode.width << 16) | (dispPtr->vMode.height);
	vgaReg[1] = (dispPtr->vMode.hps << 16) | (dispPtr->vMode.hpe);
	vgaReg[2] = (dispPtr->vMode.hpol << 16) | (dispPtr->vMode.hmax);
	vgaReg[3] = (dispPtr->vMode.vps << 16) | (dispPtr->vMode.vpe);
	vgaReg[4] = (dispPtr->vMode.vpol << 16) | (dispPtr->vMode.vmax);
	for (i = 0; i < 5; i++)
	{
		Xil_Out32(dispPtr->dispCtrlAddr + OFST_DISPLAY_VIDEO_START + (i * 4), vgaReg[i]);
	}

	/*
	 * Calculate the PLL divider parameters based on the required pixel clock frequency
	 */
	if (dispPtr->fHdmi == DISPLAY_HDMI)
	{
		pxlClkFreq = dispPtr->vMode.freq * 5;
	}
	else
	{
		pxlClkFreq = dispPtr->vMode.freq;
	}
	DisplayClkFindParams(pxlClkFreq, &clkMode);

	/*
	 * Store the obtained frequency to pxlFreq. It is possible that the PLL was not able to
	 * exactly generate the desired pixel clock, so this may differ from vMode.freq.
	 */
	dispPtr->pxlFreq = clkMode.freq;

	/*
	 * Write to the PLL dynamic configuration registers to configure it with the calculated
	 * parameters.
	 */
	DisplayClkFindReg(&clkReg, &clkMode);
	DisplayClkWriteReg(&clkReg, dispPtr->dispCtrlAddr);

	/*
	 * Enable the disp_ctrl core, which will signal the VDMA to begin transferring data
	 */
	Xil_Out32(dispPtr->dispCtrlAddr + OFST_DISPLAY_CTRL, (1 << BIT_DISPLAY_START));

	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */

/***	DisplayInitialize(DisplayCtrl *dispPtr, u16 vdmaId, u32 dispCtrlAddr, int fHdmi, u32 *framePtr[DISPLAY_NUM_FRAMES], u32 stride)
**
**	Parameters:
**		dispPtr - Pointer to the struct that will be initialized
**		vdmaId - DEVICE ID of the attached VDMA core
**		dispCtrlAddr - BASE ADDRESS of the axi_dispctrl core
**		fHdmi - flag indicating if the C_USE_BUFR_DIV5 parameter is set for the axi_dispctrl core.
**				Use DISPLAY_HDMI if it is set, otherwise use DISPLAY_NOT_HDMI
**		framePtr - array of pointers to the framebuffers. The framebuffers must be instantiated above this driver
**		stride - line stride of the framebuffers. This is the number of bytes between the start of one line and the start of another.
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Initializes the driver struct for use.
**
*/
int InitializeDisplay(DisplayCtrl *dispPtr, u32 dispCtrlAddr, u32 *framePtr)
{
	/*
	 * Initialize all the fields in the DisplayCtrl structure
	 */
	dispPtr->curFrame = 0;
	dispPtr->dispCtrlAddr = dispCtrlAddr;
	dispPtr->fHdmi = DISPLAY_NOT_HDMI;
	dispPtr->framePtr = framePtr;
	dispPtr->pxlFreq = 0;
	dispPtr->stride = VMODE_640x480.width * 4;
	dispPtr->vMode = VMODE_640x480;

	/*
	 * Initialize the VDMA Read configuration struct
	 */
	dispPtr->vdmaConfig.FrameDelay = 0;
	dispPtr->vdmaConfig.EnableCircularBuf = 1;
	dispPtr->vdmaConfig.EnableSync = 0;
	dispPtr->vdmaConfig.PointNum = 0;
	dispPtr->vdmaConfig.EnableFrameCounter = 0;

	/*
	 * Configure the VDMA to access a frame with the same dimensions as the
	 * current mode
	 */
	dispPtr->vdmaConfig.VertSizeInput = dispPtr->vMode.height;
	dispPtr->vdmaConfig.HoriSizeInput = (dispPtr->vMode.width) * 4;
	dispPtr->vdmaConfig.FixedFrameStoreAddr = dispPtr->curFrame;
	/*
	 *Also reset the stride and address values, in case the user manually changed them
	 */
	dispPtr->vdmaConfig.Stride = dispPtr->stride;
	dispPtr->vdmaConfig.FrameStoreStartAddr[0] = (u32)framePtr;

	return XST_SUCCESS;
}
/* ------------------------------------------------------------ */


u32 DisplayClkCountCalc(u32 divide)
{
	u32 output = 0;
	u32 divCalc = 0;

	divCalc = DisplayClkDivider(divide);
	if (divCalc == ERR_CLKDIVIDER)
		output = ERR_CLKCOUNTCALC;
	else
		output = (0xFFF & divCalc) | ((divCalc << 10) & 0x00C00000);
	return output;
}

u32 DisplayClkDivider(u32 divide)
{
	u32 output = 0;
	u32 highTime = 0;
	u32 lowTime = 0;

	if ((divide < 1) || (divide > 128))
		return ERR_CLKDIVIDER;

	if (divide == 1)
		return 0x1041;

	highTime = divide / 2;
	if (divide & 0b1) //if divide is odd
	{
		lowTime = highTime + 1;
		output = 1 << CLK_BIT_WEDGE;
	}
	else
	{
		lowTime = highTime;
	}

	output |= 0x03F & lowTime;
	output |= 0xFC0 & (highTime << 6);
	return output;
}

u32 DisplayClkFindReg (ClkConfig *regValues, ClkMode *clkParams)
{
	if ((clkParams->fbmult < 2) || clkParams->fbmult > 64 )
		return 0;

	regValues->clk0L = DisplayClkCountCalc(clkParams->clkdiv);
	if (regValues->clk0L == ERR_CLKCOUNTCALC)
		return 0;

	regValues->clkFBL = DisplayClkCountCalc(clkParams->fbmult);
	if (regValues->clkFBL == ERR_CLKCOUNTCALC)
		return 0;

	regValues->clkFBH_clk0H = 0;

	regValues->divclk = DisplayClkDivider(clkParams->maindiv);
	if (regValues->divclk == ERR_CLKDIVIDER)
		return 0;

	regValues->lockL = (u32) (lock_lookup[clkParams->fbmult - 1] & 0xFFFFFFFF);

	regValues->fltr_lockH = (u32) ((lock_lookup[clkParams->fbmult - 1] >> 32) & 0x000000FF);
	regValues->fltr_lockH |= ((filter_lookup_low[clkParams->fbmult - 1] << 16) & 0x03FF0000);

	return 1;
}

void DisplayClkWriteReg (ClkConfig *regValues, u32 dispCtrlAddr)
{
	Xil_Out32(dispCtrlAddr + OFST_DISPLAY_CLK_L, regValues->clk0L);
	Xil_Out32(dispCtrlAddr + OFST_DISPLAY_FB_L, regValues->clkFBL);
	Xil_Out32(dispCtrlAddr + OFST_DISPLAY_FB_H_CLK_H, regValues->clkFBH_clk0H);
	Xil_Out32(dispCtrlAddr + OFST_DISPLAY_DIV, regValues->divclk);
	Xil_Out32(dispCtrlAddr + OFST_DISPLAY_LOCK_L, regValues->lockL);
	Xil_Out32(dispCtrlAddr + OFST_DISPLAY_FLTR_LOCK_H, regValues->fltr_lockH);
}

/*
 * TODO:This function currently requires that the reference clock is 100MHz.
 * 		This should be changed so that the ref. clock can be specified, or read directly
 * 		out of hardware.
 */
double DisplayClkFindParams(double freq, ClkMode *bestPick)
{
	double bestError = 2000.0;
	double curError;
	double curClkMult;
	double curFreq;
	u32 curDiv, curFb, curClkDiv;
	u32 minFb = 0;
	u32 maxFb = 0;


	bestPick->freq = 0.0;
/*
 * TODO: replace with a smarter algorithm that doesn't doesn't check every possible combination
 */
	for (curDiv = 1; curDiv <= 10; curDiv++)
	{
		minFb = curDiv * 6; //This accounts for the 100MHz input and the 600MHz minimum VCO
		maxFb = curDiv * 12; //This accounts for the 100MHz input and the 1200MHz maximum VCO
		if (maxFb > 64)
			maxFb = 64;

		curClkMult = (100.0 / (double) curDiv) / freq; //This multiplier is used to find the best clkDiv value for each FB value

		curFb = minFb;
		while (curFb <= maxFb)
		{
			curClkDiv = (u32) ((curClkMult * (double)curFb) + 0.5);
			curFreq = ((100.0 / (double) curDiv) / (double) curClkDiv) * (double) curFb;
			curError = fabs(curFreq - freq);
			if (curError < bestError)
			{
				bestError = curError;
				bestPick->clkdiv = curClkDiv;
				bestPick->fbmult = curFb;
				bestPick->maindiv = curDiv;
				bestPick->freq = curFreq;
			}

			curFb++;
		}
	}

	return bestError;
}

/************************************************************************/


/*
 * \file	capture.c
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	Initializes and starts VDMA write channel for video capture module.
 */


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xil_types.h"
#include "xaxivdma.h"
#include "xparameters.h"

#include "capture.h"
#include "vga_modes.h"

/* ------------------------------------------------------------ */
/*				Function implementations						*/
/* ------------------------------------------------------------ */

/*
 * Initializes VDMA write channel for video capture module.
 */
void InitializeCapture(XAxiVdma_DmaSetup *vdmaSetup, u32 *framePtr)
{
	/*
	 * Initialize the VDMA Write configuration struct
	 */
	vdmaSetup->FrameDelay = 0;
	vdmaSetup->EnableCircularBuf = 1;
	vdmaSetup->EnableSync = 0;
	vdmaSetup->PointNum = 0;
	vdmaSetup->EnableFrameCounter = 0;

	/*
	 * Configure the VDMA to access a frame with the same dimensions as the
	 * current mode
	 */
	vdmaSetup->VertSizeInput = VMODE_640x480.height;
	vdmaSetup->HoriSizeInput = VMODE_640x480.width * 4;
	vdmaSetup->FixedFrameStoreAddr = 0;

	/*
	 *Also reset the stride and address values, in case the user manually changed them
	 */
	vdmaSetup->Stride = VMODE_640x480.width * 4;
	vdmaSetup->FrameStoreStartAddr[0] = (u32)framePtr;
}


/*
 * Starts VDMA write channel for video capture module.
 */
void StartCapture(XAxiVdma *vdma, XAxiVdma_DmaSetup *vdmaSetup, u32 *framePtr)
{
	/*
	 * Write channel configuration
	 */
	XAxiVdma_DmaConfig(vdma, XAXIVDMA_WRITE, vdmaSetup);
	XAxiVdma_DmaSetBufferAddr(vdma, XAXIVDMA_WRITE, vdmaSetup->FrameStoreStartAddr);
	XAxiVdma_DmaStart(vdma, XAXIVDMA_WRITE);

	// Set hardware filter parameter
	Xil_Out32(XPAR_CAPTURE_VIDEO_0_S_AXI_BASEADDR + OFFSET_CAPTURE_BRIGHTNESS, 50);
	Xil_Out32(XPAR_CAPTURE_VIDEO_0_S_AXI_BASEADDR + OFFSET_CAPTURE_CONTRAST, 0x20);
	// Start hardware filtering
	Xil_Out32(XPAR_CAPTURE_VIDEO_0_S_AXI_BASEADDR + OFFSET_CAPTURE_CONTROL, BIT_CONTROL_START | BIT_CONTROL_BRIGHTNESS_MODE);
}

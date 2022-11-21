/*
 * \file	main.c
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	Capture and display driver is initialized, image processing
 * 			function is invoked infinitely
 */

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xparameters.h"
#include "xgpiops.h"

#include "benchmarking.h"
#include "capture.h"
#include "display.h"
#include "imageProcessor.h"
#include "vdma.h"

/* ------------------------------------------------------------ */
/*				Definitions										*/
/* ------------------------------------------------------------ */

#define FRAME_SIZE (640 * 480)

/*
 * XPAR redefines
 */
#define VGA_BASEADDR XPAR_AXI_DISPCTRL_0_S_AXI_BASEADDR
#define VDMA_ID XPAR_AXIVDMA_0_DEVICE_ID
#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR
#define SW_BASEADDR XPAR_SWS_4BITS_BASEADDR
#define BTN_BASEADDR XPAR_BTNS_4BITS_BASEADDR
#define LED_BASEADDR XPAR_LEDS_4BITS_BASEADDR

// LED position is MIO07
#define LED_PIN	7

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

/*
 * Input and output frame buffers
 */
rgbType frameInput[FRAME_SIZE];
rgbType frameOutput[FRAME_SIZE];

/*
 * VDMA driver structure
 */
XAxiVdma vdma;

/* ------------------------------------------------------------ */
/*				Function implementations						*/
/* ------------------------------------------------------------ */

/*
 * Main entry function
 */
int main(void)
{
	u32 ledValue = 0;

	// Display Driver structure
	DisplayCtrl vgaCtrl;
	XAxiVdma_DmaSetup captureVdmaSetup;

	// GPIO driver structures
	XGpioPs_Config *GPIOConfigPtr;
	XGpioPs Gpio;


	//GPIO Initilization
	GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr, GPIOConfigPtr ->BaseAddr);

	XGpioPs_SetDirectionPin(&Gpio, LED_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, LED_PIN, 1);

	InitializeStatistics();

	InitializeVdma(VDMA_ID, &vdma);

	InitializeCapture(&captureVdmaSetup, (u32*)frameInput);
	InitializeDisplay(&vgaCtrl, VGA_BASEADDR, (u32*)frameOutput);

	StartCapture(&vdma, &captureVdmaSetup, (u32*)frameInput);
	StartDisplay(&vdma, &vgaCtrl, (u32*)frameOutput);

	while (1)
	{
		ledValue = !ledValue;

		StartStatistics();
		XGpioPs_WritePin(&Gpio, LED_PIN, ledValue);

		// Runs different algorithms for image processing
		RunImageProcessing(frameInput, frameOutput);

		StopStatistics();
	}
}


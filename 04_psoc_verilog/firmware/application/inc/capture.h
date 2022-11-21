/*
 * \file	capture.h
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	Initializes and starts VDMA write channel for video capture module.
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xil_types.h"
#include "xaxivdma.h"

/* ------------------------------------------------------------ */
/*				Definitions										*/
/* ------------------------------------------------------------ */

// Register address offsets
#define OFFSET_CAPTURE_CONTROL		0x0
#define OFFSET_CAPTURE_BRIGHTNESS	0x4
#define OFFSET_CAPTURE_CONTRAST		0x8

// Bit definitions for control register
#define BIT_CONTROL_START			0x01
#define BIT_CONTROL_BYPASS_MODE		0x10
#define BIT_CONTROL_SAMPLE_MODE		0x20
#define BIT_CONTROL_BRIGHTNESS_MODE	0x40
#define BIT_CONTROL_CONTRAST_MODE	0x80

/* ------------------------------------------------------------ */
/*				Global Function Declarations					*/
/* ------------------------------------------------------------ */

void InitializeCapture(XAxiVdma_DmaSetup *vdmaSetup, u32 *framePtr);
void StartCapture(XAxiVdma *vdma, XAxiVdma_DmaSetup *vdmaSetup, u32 *framePtr);


#endif /* CAPTURE_H_ */

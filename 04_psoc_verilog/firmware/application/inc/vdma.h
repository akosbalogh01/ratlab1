/*
 * \file	vdma.h
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	Initializes VDMA driver.
 */


#ifndef VDMA_H_
#define VDMA_H_

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xil_types.h"
#include "vga_modes.h"
#include "xaxivdma.h"

/* ------------------------------------------------------------ */
/*				Global Function Declarations					*/
/* ------------------------------------------------------------ */

void InitializeVdma(u16 vdmaId, XAxiVdma *vdma);


#endif /* VDMA_H_ */

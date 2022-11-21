/*
 * \file	vdma.c
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	Initializes VDMA driver.
 */


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xil_types.h"
#include "xaxivdma.h"

/* ------------------------------------------------------------ */
/*				Function implementations						*/
/* ------------------------------------------------------------ */

/*
 * Initializes VDMA driver for both (read / write) channels
 */
void InitializeVdma(u16 vdmaId, XAxiVdma *vdma)
{
	XAxiVdma_Config *Config;

	Config = XAxiVdma_LookupConfig(vdmaId);
	XAxiVdma_CfgInitialize(vdma, Config, Config->BaseAddress);
}

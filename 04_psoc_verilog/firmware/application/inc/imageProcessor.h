/*
 * \file	imageProcessor.h
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	Different image processor functions.
 */

#ifndef IMAGEPROCESSOR_H_
#define IMAGEPROCESSOR_H_


#include "arm_neon.h"

/* ------------------------------------------------------------ */
/*				General Type Declarations						*/
/* ------------------------------------------------------------ */

typedef struct
{
	u8 blue;
	u8 green;
	u8 red;
	u8 unused;
} rgbType;


void RunImageProcessing(rgbType *frameInput, rgbType *frameOutput);

#endif /* IMAGEPROCESSOR_H_ */

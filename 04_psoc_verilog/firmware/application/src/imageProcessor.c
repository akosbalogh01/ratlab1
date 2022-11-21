/*
 * \file	imageProcessor.c
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	Different image processor functions.
 */


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <limits.h>

#include "arm_neon.h"

#include "xil_types.h"
#include "xil_cache.h"

#include "imageProcessor.h"
#include "vga_modes.h"

/* ------------------------------------------------------------ */
/*				Global constants								*/
/* ------------------------------------------------------------ */

static const s8 filterEdge[3][3] =
{
	{-1, -1, -1},
	{-1,  8, -1},
	{-1, -1, -1}
};

static const s8 filterEmboss[3][3] =
{
	{-2, -1,  0},
	{-1,  1,  1},
	{ 0,  1,  2}
};

/* ------------------------------------------------------------ */
/*				Local function declarations						*/
/* ------------------------------------------------------------ */

// Unused attribute suppresses warning if function is not used
static void BypassImage(rgbType* frameInput, rgbType* frameOutput) __attribute__((unused));
static void BypassImageNeon(rgbType* frameInput, rgbType* frameOutput) __attribute__((unused));

static void SampleImageProcessing(rgbType* frameInput, rgbType* frameOutput) __attribute__((unused));
static void SampleImageProcessingNeon(rgbType* frameInput, rgbType* frameOutput) __attribute__((unused));

/* ------------------------------------------------------------ */
/*				Function implementations						*/
/* ------------------------------------------------------------ */

/*
 * Wrapper function for different image processing algorithms
 */
void RunImageProcessing(rgbType* frameInput, rgbType* frameOutput)
{
	BypassImage(frameInput, frameOutput);
	//BypassImageNeon(frameInput, frameOutput);
	//SampleImageProcessing(frameInput, frameOutput);
	//SampleImageProcessingNeon(frameInput, frameOutput);
}


/*
 * Bypasses incoming images.
 *
 * Runtime (-O3, -fno-tree-vectorize, -fno-prefetch-loop-arrays): 3.87ms
 */
static void BypassImage(rgbType* frameInput, rgbType* frameOutput)
{
	u32 i;

	for (i = 0; i < VMODE_640x480.size; i++)
	{	// Plain pixel copy
		frameOutput[i] = frameInput[i];
	}
}


/*
 * Bypasses incoming images with NEON instructions.
 *
 * Runtime (-O3): 2.85ms
 */
static void BypassImageNeon(rgbType* frameInput, rgbType* frameOutput)
{
	// Vector variable
	uint8x16x4_t pixelVectorArray;

	u32 i;

	for (i = 0; i < VMODE_640x480.size; i += 16)
	{	// Vectorized pixel copy
		pixelVectorArray = vld4q_u8((u8*)&frameInput[i]);
		vst4q_u8((u8*)&frameOutput[i], pixelVectorArray);
	}
}


/*
 * Sample image processing code. It eliminates green channel.
 *
 * Runtime (-O3, -fno-tree-vectorize, -fno-prefetch-loop-arrays): 4.80ms
 */
static void SampleImageProcessing(rgbType* frameInput, rgbType* frameOutput)
{
	u32 i;

	for (i = 0; i < VMODE_640x480.size; i++)
	{	// Red and blue channels are unmodified
		// Green channel is set to 0
		frameOutput[i].red = frameInput[i].red;
		frameOutput[i].green = 0;
		frameOutput[i].blue = frameInput[i].blue;
	}
}


/*
 * Sample image processing code with NEON instructions. It eliminates green channel.
 *
 * Runtime (-O3): 2.91ms
 */
static void SampleImageProcessingNeon(rgbType* frameInput, rgbType* frameOutput)
{
	// Vector variable
	uint8x16x4_t pixelVectorArray;

	u32 i;

	for (i = 0; i < VMODE_640x480.size; i += 16)
	{
		// Load pixel vector
		pixelVectorArray = vld4q_u8((u8*)&frameInput[i]);

		// Vector containing green channels is set to zero
		pixelVectorArray.val[1] = vdupq_n_u8(0);

		// Copy the result back to DDRAM
		vst4q_u8((u8*)&frameOutput[i], pixelVectorArray);
	}
}

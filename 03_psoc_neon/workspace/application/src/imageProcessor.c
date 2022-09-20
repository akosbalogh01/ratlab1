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
#include <stdbool.h>

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

static void ColorOffsetImage(rgbType* frameInput, rgbType* frameOutput, int8_t offset);
static void ColorOffsetImageNeon(rgbType* frameInput, rgbType* frameOutput, int8_t offset);
static void ContrastImage(rgbType* frameInput, rgbType* frameOutput, int8_t cf);
static void ContrastImageNeon(rgbType* frameInput, rgbType* frameOutput, int8_t cf);
static void ConvFilterImage(rgbType* frameInput, rgbType* frameOutput, s8 filter[3][3]);

static void SampleImageProcessing(rgbType* frameInput, rgbType* frameOutput) __attribute__((unused));
static void SampleImageProcessingNeon(rgbType* frameInput, rgbType* frameOutput) __attribute__((unused));

static uint8_t sat(int a)
{
	if (a > 255)
		return 255;
	else if (a < 0)
		return 0;
	return a;
}

/* ------------------------------------------------------------ */
/*				Function implementations						*/
/* ------------------------------------------------------------ */

/*
 * Wrapper function for different image processing algorithms
 */
void RunImageProcessing(rgbType* frameInput, rgbType* frameOutput)
{
	//BypassImage(frameInput, frameOutput);
	// ColorOffsetImage(frameInput, frameOutput, -100);
	// ColorOffsetImageNeon(frameInput, frameOutput, -127);
	//ContrastImageNeon(frameInput, frameOutput, 0b00100000);
	ConvFilterImage(frameInput, frameOutput, filterEdge);

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
 * Bypasses incoming images.
 *
 * Runtime (-O3, -fno-tree-vectorize, -fno-prefetch-loop-arrays): 3.87ms
 */
static void ColorOffsetImage(rgbType* frameInput, rgbType* frameOutput, int8_t offset)
{
	u32 i;

	for (i = 0; i < VMODE_640x480.size; i++)
	{	// Plain pixel copy
		frameOutput[i].red   = sat((uint16_t) frameInput[i].red + offset);
		frameOutput[i].green = sat((uint16_t) frameInput[i].green + offset);
		frameOutput[i].blue  = sat((uint16_t) frameInput[i].blue + offset);
	}
}

static void ColorOffsetImageNeon(rgbType* frameInput, rgbType* frameOutput, int8_t offset)
{
	bool is_negative = false;
	if (offset < 0)
	{
		is_negative = true;
		offset = (-1) * offset;
	}

	// Vector variable
	uint8x16x4_t pixelVectorArray;
	uint8x16_t offsetVector;

	u32 i;

	for (i = 0; i < VMODE_640x480.size; i += 16)
	{	// Vectorized pixel copy
		pixelVectorArray = vld4q_u8((u8*)&frameInput[i]);
		offsetVector = vdupq_n_u8(offset);
		if (is_negative == true)
		{
			pixelVectorArray.val[0] = vqsubq_u8(pixelVectorArray.val[0], offsetVector);
			pixelVectorArray.val[1] = vqsubq_u8(pixelVectorArray.val[1], offsetVector);
			pixelVectorArray.val[2] = vqsubq_u8(pixelVectorArray.val[2], offsetVector);
		}
		else
		{
			pixelVectorArray.val[0] = vqaddq_u8(pixelVectorArray.val[0], offsetVector);
			pixelVectorArray.val[1] = vqaddq_u8(pixelVectorArray.val[1], offsetVector);
			pixelVectorArray.val[2] = vqaddq_u8(pixelVectorArray.val[2], offsetVector);
		}

		vst4q_u8((u8*)&frameOutput[i], pixelVectorArray);

	}
}

static void ContrastImage(rgbType* frameInput, rgbType* frameOutput, int8_t cf)
{
	u32 i;
/*
 * :)
	uint8_t dec  = (cf >> 4);
	uint8_t frac = (cf & 0x0F);
*/
	for (i = 0; i < VMODE_640x480.size; i++)
	{	// Plain pixel copy
		frameOutput[i].red   = sat((((frameInput[i].red - 128)*cf) >> 4) + 128);
		frameOutput[i].green = sat((((frameInput[i].green - 128)*cf) >> 4) + 128);
		frameOutput[i].blue  = sat((((frameInput[i].blue - 128)*cf) >> 4) + 128);
	}
}

static void ContrastImageNeon(rgbType* frameInput, rgbType* frameOutput, int8_t factor)
{
	// Vector variable
	uint8x8x4_t pixelVectorArray;
	int16x8_t factorVector, redVector, greenVector, blueVector;
	int16x8_t tempVector;

	u32 i;
	u32 j;

	for (i = 0; i < VMODE_640x480.size; i += 8)
	{	// Vectorized pixel copy
		pixelVectorArray = vld4_u8((u8*)&frameInput[i]);
		factorVector = vdupq_n_s16(factor);

		for (j = 0; j < 3; j++)
		{
			tempVector = (int16x8_t)vmovl_u8(pixelVectorArray.val[j]);

			tempVector = vsubq_s16(tempVector, vdupq_n_s16(128));

			tempVector = vmulq_s16(tempVector, factorVector);
			tempVector = vshrq_n_s16(tempVector, 4);
			tempVector = vaddq_s16(tempVector, vdupq_n_s16(128));

			pixelVectorArray.val[j] = vqmovun_s16(tempVector);
		}

		vst4_u8((u8*)&frameOutput[i], pixelVectorArray);

	}
}


static void ConvFilterImage(rgbType* frameInput, rgbType* frameOutput, s8 filter[3][3])
{
	u32 y, x;
	s32 fy, fx;

	for (y = 1; y < VMODE_640x480.height - 1; y++)
	{
		for (x = 1; x < VMODE_640x480.width - 1; x++)
		{
			s32 accu[3] = {0};

			for (fy = -1; fy <= 1; fy++)
			{
				for (fx = -1; fx <= 1; fx++)
				{
					accu[0] = accu[0] + frameInput[(y+fy) * VMODE_640x480.width + (x+fx)].red * filter[fy+1][fx+1];
					accu[1] = accu[1] + frameInput[(y+fy) * VMODE_640x480.width + (x+fx)].green * filter[fy+1][fx+1];
					accu[2] = accu[2] + frameInput[(y+fy) * VMODE_640x480.width + (x+fx)].blue * filter[fy+1][fx+1];
				}
			}

			frameOutput[y * VMODE_640x480.width + x].red = sat(accu[0]);
			frameOutput[y * VMODE_640x480.width + x].green = sat(accu[1]);
			frameOutput[y * VMODE_640x480.width + x].blue = sat(accu[2]);
		}
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

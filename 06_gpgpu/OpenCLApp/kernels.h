#pragma once

#define MODE_OPENCL 0
#define MODE_CPU 1


#include <time.h>
#include <stdlib.h>
#include <string.h>


#include "openclhelper.h"

//Entry points for the tasks
void KernelAddRun(int mode, int platformindex, int deviceindex, int repeatcount);
void KernelSumReduceRun(int mode, int platformindex, int deviceindex, int repeatcount);
void KernelBrightnessRun(int mode, int platformindex, int deviceindex, int repeatcount, unsigned char brightness);
void KernelContrastRun(int mode, int platformindex, int deviceindex, int repeatcount, unsigned char contrast);
void KernelDebayerRun(int mode, int platformindex, int deviceindex, int repeatcount);
void KernelBlurRun(int mode, int platformindex, int deviceindex, int repeatcount, int kernelsize, float sigma);
// OpenCLApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include "include/CL/opencl.h"


#include "kernels.h"
#include <stdio.h>

#include "bmp.h"
#include "openclhelper.h"


int main()
{
    clPrint();

    int mode = 0;
    int platformindex = 0;
    int deviceindex = 0;
    int kernel = 0;

    printf("\r\nSelect mode: 0: OpenCL, 1: CPU ");
    scanf_s("%d", &mode);

    if (mode == MODE_OPENCL) {
        printf("\r\nSelect platform: ");
        scanf_s("%d", &platformindex);

        printf("\r\nSelect device: ");
        scanf_s("%d", &deviceindex);
    }
    else if (mode == MODE_CPU) {

    }
    else {
        printf("\r\nUnknown mode, exiting");
        return 1;
    }
    
    printf("\r\nKernel 0: Vector add");
    printf("\r\nKernel 1: Sum reduce");
    printf("\r\nKernel 2: Image brightness");
    printf("\r\nKernel 3: Image contrast");
    printf("\r\nKernel 4: Image debayer");
    printf("\r\nKernel 5: Image blur");

    
    printf("\r\nSelect kernel: ");
    scanf_s("%d", &kernel);

    switch (kernel) {
    case 0:
        KernelAddRun(mode, platformindex, deviceindex, 1);
        break;
    case 1:
        KernelSumReduceRun(mode, platformindex, deviceindex, 1); 
        break;
    case 2:
        KernelBrightnessRun(mode, platformindex, deviceindex, 1, 200);
        break;
    case 3:
        KernelContrastRun(mode, platformindex, deviceindex, 1, 200);
        break;
    case 4:
        KernelDebayerRun(mode, platformindex, deviceindex, 1);
        break;
    case 5:
        KernelBlurRun(mode, platformindex, deviceindex, 1,10,4);
        break;
    default:
        printf("Unknown kernel");
    }
    /*
    char s[10];
    scanf_s("%9s", s, 10);
    */
    printf("Press ENTER key to Continue\n");
    getchar();
}

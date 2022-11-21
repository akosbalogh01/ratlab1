#pragma once

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include "include/CL/opencl.h"

//OpenCL context descriptor
typedef struct _myopencl {
	cl_command_queue commandQueue;
	cl_kernel kernel;
	cl_program program;
	cl_context context;
	cl_device_id* deviceIds;
	size_t workgroup_size;
	size_t workitem_size[3];
} OpenCL_t, *pOpenCL_t;


void clPrint();

//Init OpenCL platform
//Example:
//int platform_index = 0;
//int device_index = 0;
//cl_device_type device_type = CL_DEVICE_TYPE_GPU;
//OpenCL_t opencl;
//cl_int status;
//int error = clInitPlatform(platform_index, device_index, device_type, "mykernel.cl", "mykernel", NULL, &opencl);

int clInitPlatform(int platform_index, int device_index, cl_device_type device_type, const char* kernelFilename, const char* kernelName, const char* buildOptions, pOpenCL_t pOpenCL);


//Release resources
//Example:
//	clCleanup(&opencl);

void clCleanup(pOpenCL_t popencl);


int MakeDivisableUp(int value, int divisor);
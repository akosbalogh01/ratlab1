
#define OPENCLVERSION 12

#include "openclhelper.h"
#include <stdio.h>
#include <stdlib.h>

//Print OpenCL platforms and devices
void clPrint() {

    cl_uint numPlatforms;
    cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clGetPlatformIDs");
        return 1;
    }
    printf("\r\nNumber of platforms: %d", numPlatforms);
    cl_platform_id* platforms = NULL;

    if (numPlatforms <= 0)
    {
        printf("\nError: numPlatforms==0");
        return -2;
    }
    platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));

    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clGetPlatformIDs");
        free(platforms);
        return -3;
    }

    for (int platform_index = 0; platform_index < numPlatforms; platform_index++) {

        cl_platform_id platform = platforms[platform_index];

        // Get platform name
        char platformVendor[1024];
        status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
        if (status != CL_SUCCESS) {
            printf("\r\nError: clGetPlatformInfo");
            free(platforms);
            return -4;
        }

        printf("\r\nPlatform index: %d Vendor : %s", platform_index, platformVendor);

        // Get number of devices available
        cl_uint deviceCount = 0;
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        if (status != CL_SUCCESS) {
            printf("\r\nError: clGetDeviceIDs");
            free(platforms);
            return -5;
        }


        cl_device_id* deviceIds = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);

        // Get device ids
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, deviceCount, deviceIds, NULL);
        if (status != CL_SUCCESS) {
            printf("\r\nError: clGetDeviceIDs");
            free(platforms);
            free(deviceIds);
            return -5;
        }

        // Print device index and device names
        for (cl_uint i = 0; i < deviceCount; ++i)
        {
            char deviceName[1024];
            status = clGetDeviceInfo(deviceIds[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
            if (status != CL_SUCCESS) {
                printf("\r\nError: clGetDeviceInfo");
                free(platforms);
                free(deviceIds);
                return -6;
            }

            size_t workgroup_size;
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);

            size_t workitem_size[3];
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);

            printf("\r\nDevice index %d : %s Device ID is %d", i, deviceName, deviceIds[i]);
            
            printf("\r\n  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t%u\n", workgroup_size);
            printf("\r\n  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t%u / %u / %u \n", workitem_size[0], workitem_size[1], workitem_size[2]);





        }

        free(deviceIds);
        printf("\r\n------------------------------");
    }
    free(platforms);
}

//Init a given Opencl platform and device
int clInitPlatform(int platform_index, int device_index, cl_device_type device_type, const char*kernelFilename, const char* kernelName, const char* buildOptions, pOpenCL_t pOpenCL) {

    pOpenCL->commandQueue = NULL;
    pOpenCL->context = NULL;
    pOpenCL->kernel = NULL;
    pOpenCL->program = NULL;


    cl_uint numPlatforms;
    cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clGetPlatformIDs");
        return 1;
    }
    cl_platform_id* platforms = NULL;
   

    if (numPlatforms<=0)
    {
        printf("\nError: numPlatforms==0");
        return -2;
    }
    platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));

    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clGetPlatformIDs");
        free(platforms);
        return -3;
    }

    cl_platform_id platform = platforms[platform_index];

    // Get platform name
    char platformVendor[1024];
    status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clGetPlatformInfo");
        free(platforms);
        return -4;
    }

    printf("\r\nSelected Platform Vendor : %s", platformVendor);

    //#define VERSION_LENGTH 64
    //char complete_version[VERSION_LENGTH];
    //size_t realSize = 0;
    //clGetPlatformInfo(platform, CL_PLATFORM_VERSION, VERSION_LENGTH,
    //    &complete_version, &realSize);
    //char version[4];
    //version[3] = 0;
    //memcpy(version, &complete_version[7], 3);
    //// printf("V %s %f\n", version, version_float);
    //float version_float = atof(version);


    // Get number of devices available
    cl_uint deviceCount = 0;
    status = clGetDeviceIDs(platform, device_type, 0, NULL, &deviceCount);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clGetDeviceIDs");
        free(platforms);
        return -5;
    }

    cl_device_id* deviceIds = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
    pOpenCL->deviceIds = deviceIds;

    // Get device ids
    status = clGetDeviceIDs(platform, device_type, deviceCount, deviceIds, NULL);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clGetDeviceIDs");
        free(platforms);
        free(deviceIds);
        return -5;
    }

    clGetDeviceInfo(deviceIds[device_index], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(pOpenCL->workgroup_size), &pOpenCL->workgroup_size, NULL);

    clGetDeviceInfo(deviceIds[device_index], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(pOpenCL->workitem_size), &pOpenCL->workitem_size, NULL);



    // Print device index and device names
   /* for (cl_uint i = 0; i < deviceCount; ++i)
    {
        char deviceName[1024];
        status = clGetDeviceInfo(deviceIds[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
        if (status != CL_SUCCESS) {
            printf("\r\nError: clGetDeviceInfo");
            free(platforms);
            free(deviceIds);
            return -6;
        }
        printf("Device %d : %s Device ID is %d", i, deviceName, deviceIds[i]);
    }*/

    cl_context_properties cps[3] =
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)platform,
        0
    };

    cl_context context = clCreateContext(
        cps,
        1,
        &deviceIds[device_index],
        NULL,
        NULL,
        &status);

    //free(platforms);

    if (status != CL_SUCCESS) {
        printf("\r\nError: clCreateContext");
        free(deviceIds);
        return -7;
    }
    pOpenCL->context = context;

#if OPENCLVERSION==20 
        cl_command_queue_properties prop = 0;
        cl_command_queue commandQueue = clCreateCommandQueueWithProperties(
            context,
            deviceIds[device_index],
            &prop,
            &status);
        if (status != CL_SUCCESS) {
            printf("\r\nError: clCreateCommandQueueWithProperties");

            commandQueue = clCreateCommandQueue(context, deviceIds[device_index], 0, &status);
            if (status != CL_SUCCESS) {
                free(deviceIds);
                printf("\r\nError: clCreateCommandQueue");
                return -8;
            }
        }
#else
    cl_command_queue commandQueue = clCreateCommandQueue(context, deviceIds[device_index], 0, &status);
    if (status != CL_SUCCESS) {
        free(deviceIds);
        printf("\r\nError: clCreateCommandQueue");
        return -8;
    }

#endif
    


    pOpenCL->commandQueue = commandQueue;

    FILE* fp;
    char* source_str;
    size_t source_size;

    fopen_s(&fp, kernelFilename, "r");
    if (!fp) {
        printf("\r\nFailed to load kernel.");
        free(deviceIds);
        return -9;
    }

    fseek(fp, 0, SEEK_END); // seek to end of file
    int filesize = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET);

    source_str = (char*)malloc(filesize);
    source_size = fread(source_str, 1, filesize, fp);
    fclose(fp);


    cl_program program = clCreateProgramWithSource(context,
        1,
        (const char**)&source_str,
        (const size_t*)&source_size,
        &status);
    free(source_str);

    if (status != CL_SUCCESS) {
        printf("\r\nError: clCreateProgramWithSource");
        free(deviceIds);
        return -9;
    }
   


    status = clBuildProgram(program, 1, &deviceIds[device_index], buildOptions, NULL, NULL);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clBuildProgram");
        free(deviceIds);
        return -10;
    }


    cl_kernel kernel = clCreateKernel(program, kernelName, &status);
    if (status != CL_SUCCESS) {
        printf("\r\nError: clCreateKernel");
        free(deviceIds);
        return -11;
    }  

/*    size_t kernelglobalworksize[3] = {1024,1,1};
    size_t kernelglobalworksizesize;
    int ppp = sizeof(kernelglobalworksize);
    size_t kernelworkgroupsize;
    status = clGetKernelWorkGroupInfo(kernel, deviceIds[device_index], CL_KERNEL_GLOBAL_WORK_SIZE, sizeof(kernelglobalworksize), &kernelglobalworksize, &kernelglobalworksizesize);
    status = clGetKernelWorkGroupInfo(kernel, deviceIds[device_index], CL_KERNEL_WORK_GROUP_SIZE, sizeof(kernelworkgroupsize), &kernelworkgroupsize, NULL);
    */
    free(deviceIds);


    pOpenCL->kernel = kernel;
    pOpenCL->program = program;

    return 0;
}


//Clean up OpenCL resources
void clCleanup(pOpenCL_t pOpenCL) {
    cl_int status;
    if (pOpenCL->commandQueue != NULL) {
        status = clFlush(pOpenCL->commandQueue);
        status = clFinish(pOpenCL->commandQueue);
    }

    if (pOpenCL->kernel != NULL) {
        status = clReleaseKernel(pOpenCL->kernel);
        pOpenCL->kernel = NULL;
    }


    if (pOpenCL->program != NULL) {
        status = clReleaseProgram(pOpenCL->program);
        pOpenCL->program = NULL;
    }

    if (pOpenCL->commandQueue != NULL) {
        status = clReleaseCommandQueue(pOpenCL->commandQueue);
        pOpenCL->commandQueue = NULL;
    }
    if (pOpenCL->context != NULL) {
        status = clReleaseContext(pOpenCL->context);
        pOpenCL->context = NULL;
    }


}


int MakeDivisableUp(int value, int divisor) {
    if (value % divisor == 0) {
        return value;
    }
    return value + divisor - value % divisor;
}
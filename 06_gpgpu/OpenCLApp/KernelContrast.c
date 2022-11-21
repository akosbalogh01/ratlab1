#include "kernels.h"
#include "bmp.h"

//Descriptor for the task
typedef struct KernelData_ {

	bmp_t InputBmp;
	bmp_t OutputBmp;
	int ImageWidth;
	int ImageHeight;
	unsigned char Contrast;
	int ImageSizeBytes;
	int ImageSizePixels;
	OpenCL_t OpenCL;
	int RepeatCount;
	clock_t timeBegin;
	clock_t timeEnd;

} KernelData, * pKernelData;


//Free up kernel and OpenCL resources
static void FreeKernel(pKernelData pdata) {

	BmpClose(&pdata->InputBmp);
	BmpClose(&pdata->OutputBmp);

	free(pdata);
}

//Initialize task parameters
static pKernelData InitKernel() {
	pKernelData pdata = (pKernelData)malloc(sizeof(struct KernelData_));
	pdata->RepeatCount = 1;

	int err = BmpLoad("cica.bmp", &pdata->InputBmp);

	if (err != 0) {
		printf("Error loading image");
		free(pdata);
		return NULL;
	}
	pdata->ImageSizeBytes = pdata->InputBmp.totalbytes;
	pdata->ImageWidth = pdata->InputBmp.width;
	pdata->ImageHeight = pdata->InputBmp.height;
	pdata->ImageSizePixels = pdata->InputBmp.width * pdata->InputBmp.height;

	BmpCopyNew(&pdata->OutputBmp, &pdata->InputBmp);

	pdata->timeBegin = clock();
	pdata->timeEnd = clock();

	return pdata;
}


//Implement the function in C running on the CPU for verification
static void Kernel_CPU(int imagesize, unsigned int* image, const unsigned char contrast) {
	//TODO 4
	//update image pixels
	//use contrast parameter
	//imagesize: number of pixels

}

//Verify results
static int Kernel_Verify(int imagesize, unsigned int* image, unsigned int* imagetest, unsigned char contrast) {
	unsigned int* imagetmp = (unsigned int*)malloc(imagesize * 4);
	memcpy(imagetmp, image, imagesize * 4);
	Kernel_CPU(imagesize, imagetmp, contrast);

	int result = memcmp(imagetmp, imagetest, imagesize * 4);

	free(imagetmp);
	return result;
}

//Run the task using OpenCL
static void Kernel_OpenCL(pKernelData pdata) {

	cl_int status;

	//TODO 4
	//Create the buffers

	//Define workitem and workgroup sizes
	
	//Set kernel arguments
	

	pdata->timeBegin = clock();
	for (int i = 0; i < pdata->RepeatCount; i++) {

		//TODO 4
		//copy data from host to device

		//Enqueue kernel

		//Read back data from device to host

	}

	pdata->timeEnd = clock();


	//Verify results
	int testresult = 0;
	testresult = Kernel_Verify(pdata->ImageSizePixels, pdata->InputBmp.pdata, pdata->OutputBmp.pdata, pdata->Contrast);
	if (testresult != 0) {
		printf("\r\nTest ERROR!");
	}

	if (testresult == 0) {
		printf("\r\nTest OK!");
	}


	//TODO 4 
	//Free up resources

}



//Run the selected task
void KernelContrastRun(int mode, int platformindex, int deviceindex, int repeatcount, unsigned char contrast) {
	//Init Data
	pKernelData pdata = InitKernel();
	if (pdata == NULL) {
		return;
	}
	pdata->RepeatCount = repeatcount;
	pdata->Contrast = contrast;

	if (mode == MODE_CPU) {
		//Run on CPU
		pdata->timeBegin = clock();
		for (int i = 0; i < pdata->RepeatCount; i++) {
			Kernel_CPU(pdata->ImageSizePixels, pdata->OutputBmp.pdata, pdata->Contrast);
		}
		pdata->timeEnd = clock();
	}

	if (mode == MODE_OPENCL) {
		//Run using OpenCL
		cl_int status;
		status = clInitPlatform(platformindex, deviceindex, CL_DEVICE_TYPE_ALL, "kernel_contrast.cl", "contrastrow", NULL, &pdata->OpenCL);
		if (status != 0) {
			printf("Error in opencl init: %d", status);
			FreeKernel(pdata);
			return;
		}
		Kernel_OpenCL(pdata);

		clCleanup(&pdata->OpenCL);
	}

	BmpSave("contrast.bmp", &pdata->OutputBmp);

	double time_spent = (double)(pdata->timeEnd - pdata->timeBegin) / CLOCKS_PER_SEC * 1000 / pdata->RepeatCount;

	printf("\r\n %d Iterations, Mean iteration time: %f ms", pdata->RepeatCount, time_spent);

	FreeKernel(pdata);
}


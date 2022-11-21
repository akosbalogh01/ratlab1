#include "kernels.h"
#include "bmp.h"

//Descriptor for the task
typedef struct KernelData_ {

	bmp_t InputBmp;
	bmp_t OutputBmp;
	int ImageWidth;
	int ImageHeight;
	unsigned char Brightness;
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
	pdata->ImageHeight =  pdata->InputBmp.height;
	pdata->ImageSizePixels= pdata->InputBmp.width* pdata->InputBmp.height;

	BmpCopyNew(&pdata->OutputBmp, &pdata->InputBmp);

	pdata->timeBegin = clock();
	pdata->timeEnd = clock();

	return pdata;
}


//Implement the function in C running on the CPU for verification
static void Kernel_CPU(int imagesize, unsigned int* image, const unsigned char brightness) {
	//TODO 3
	//update image pixels
	//use brightness parameter
	//imagesize: number of pixels

	int rgb[4] = { 0 };
	for (int i = 0; i < imagesize; i++)
	{
		for (int n = 0; n < 3; n++) {
			rgb[n]=(image[i] & (0x000000FF << (n*8)))>>(n*8);
			rgb[n] = rgb[n] - (128 - brightness);
			if (rgb[n] > 255) rgb[n] = 255;
			if (rgb[n] < 0) rgb[n] = 0;
		}
		rgb[3] = (image[i] & (0x000000FF << (3 * 8))) >> (3 * 8);
		image[i] = 0;
		for (int n = 0; n < 4; n++) {
			image[i] |= rgb[n] << (n * 8);
		}
	}
}


//Verify results
static int Kernel_Verify(int imagesize, unsigned int* image, unsigned int* imagetest, unsigned char brightness) {
	unsigned int* imagetmp = (unsigned int*)malloc(imagesize * 4);
	memcpy(imagetmp, image, imagesize * 4);
	Kernel_CPU(imagesize, imagetmp, brightness);

	int result = memcmp(imagetmp, imagetest, imagesize*4);

	free(imagetmp);
	return result;
}

//Run the task using OpenCL
static void Kernel_OpenCL(pKernelData pdata) {

	cl_int status;

	//Create the buffers

	cl_mem climage = clCreateBuffer(
		pdata->OpenCL.context,
		CL_MEM_READ_WRITE,//| CL_MEM_COPY_HOST_PTR
		sizeof(cl_int) * pdata->ImageSizeBytes,
		NULL,//pdata->InputData1
		&status);

	//Define workitem and workgroup sizes
	size_t local_item_size = pdata->OpenCL.workgroup_size; // 256 //64
	size_t global_item_size =MakeDivisableUp(pdata->ImageSizePixels, local_item_size);

	//Set kernel arguments
	status = clSetKernelArg(pdata->OpenCL.kernel, 0, sizeof(cl_mem), (void*)&climage);
	status = clSetKernelArg(pdata->OpenCL.kernel, 1, sizeof(cl_uchar), (void*)&pdata->Brightness);
	status = clSetKernelArg(pdata->OpenCL.kernel, 2, sizeof(cl_int), (void*)&pdata->ImageSizePixels);


	pdata->timeBegin = clock();
	for (int i = 0; i < pdata->RepeatCount; i++) {

		//TODO 3

		//copy data from host to device

		//Enqueue kernel

		//Read back data from device to host

	}

	pdata->timeEnd = clock();


	//Verify results
	int testresult = 0;
	testresult = Kernel_Verify(pdata->ImageSizePixels, pdata->InputBmp.pdata, pdata->OutputBmp.pdata, pdata->Brightness);
	if (testresult != 0) {
		printf("\r\nTest ERROR!");
	}

	if (testresult == 0) {
		printf("\r\nTest OK!");
	}

	//TODO 3
	//Free up resources


}


//Run the selected task
void KernelBrightnessRun(int mode, int platformindex, int deviceindex, int repeatcount, unsigned char brightness) {

	//Init Data
	pKernelData pdata = InitKernel();
	if (pdata == NULL) {
		return;
	}
	pdata->RepeatCount = repeatcount;
	pdata->Brightness = brightness;

	if (mode == MODE_CPU) {
		//Run on CPU
		pdata->timeBegin = clock();
		for (int i = 0; i < pdata->RepeatCount; i++) {
			Kernel_CPU(pdata->ImageSizePixels, pdata->OutputBmp.pdata, pdata->Brightness);
		}
		pdata->timeEnd = clock();
	}

	if (mode == MODE_OPENCL) {
		//Run using OpenCL
		cl_int status;
		status = clInitPlatform(platformindex, deviceindex, CL_DEVICE_TYPE_ALL, "kernel_brightness.cl", "brightness", NULL, &pdata->OpenCL);
		if (status != 0) {
			printf("Error in opencl init: %d", status);
			FreeKernel(pdata);
			return;
		}
		Kernel_OpenCL(pdata);

		clCleanup(&pdata->OpenCL);
	}

	BmpSave("brightness.bmp", &pdata->OutputBmp);

	double time_spent = (double)(pdata->timeEnd - pdata->timeBegin) / CLOCKS_PER_SEC * 1000 / pdata->RepeatCount;

	printf("\r\n %d Iterations, Mean iteration time: %f ms", pdata->RepeatCount, time_spent);

	FreeKernel(pdata);
}


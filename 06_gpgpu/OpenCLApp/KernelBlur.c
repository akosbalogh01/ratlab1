#include <math.h>
#include "kernels.h"
#include "bmp.h"

#define M_PI 3.14159265358979323846

//Descriptor for the task
typedef struct KernelData_ {

	bmp_t InputBmp;
	bmp_t OutputBmp;
	float* GaussianKernel;
	int GaussianKernelSize;
	OpenCL_t OpenCL;
	int RepeatCount;
	clock_t timeBegin;
	clock_t timeEnd;

} KernelData, * pKernelData;

//Free up kernel and OpenCL resources
static void FreeKernel(pKernelData pdata) {

	BmpClose(&pdata->InputBmp);
	BmpClose(&pdata->OutputBmp);

	free(pdata->GaussianKernel);
	free(pdata);
}

//Create blur matrix data
void SetupBlurMatrix(float sigma, int size, float** matrix) {
	int totalsize = (size * 2 + 1);
	*matrix = (float*)malloc(totalsize * totalsize * sizeof(float));

	for (int i = -size; i <= size; i++) {
		for (int j = -size; j <= size; j++) {
			int u = i + size;
			int v = j + size;
			float w = 1.0f / (2 * M_PI * sigma * sigma) * exp(-1 * ((i * i) + (j * j)) / (2 * sigma * sigma));
			(*matrix)[u + v * totalsize] = w;
		}
	}

}

//Initialize task parameters
static pKernelData InitKernel(float sigma, int gaussiansize) {
	pKernelData pdata = (pKernelData)malloc(sizeof(struct KernelData_));
	pdata->RepeatCount = 1000;

	SetupBlurMatrix(sigma, gaussiansize, &pdata->GaussianKernel);
	pdata->GaussianKernelSize = gaussiansize;

	int err = BmpLoad("cica.bmp", &pdata->InputBmp);

	if (err != 0) {
		printf("Error loading image");
		free(pdata);
		return NULL;
	}
	BmpNew(pdata->InputBmp.width, pdata->InputBmp.height, &pdata->OutputBmp);

	pdata->timeBegin = clock();
	pdata->timeEnd = clock();

	return pdata;
}



//Implement the function in C running on the CPU for verification
static void Kernel_CPU(pbmp_t imagein, pbmp_t imageout, int matrixsize, float* matrix) {
	//TODO 6
	//implement convolution for each pixel
	int totalsize = (matrixsize * 2 + 1);

	for (int y = 0; y < imagein->height; y++)
	{
		for (int x = 0; x < imagein->width; x++)
		{
			float rgb[4] = { 0 };
			unsigned int cnt = 0;
			float weight_sum = 0;
			for (int i = -matrixsize; i <= matrixsize; i++)
			{
				for (int j = -matrixsize; j <= matrixsize; j++)
				{

					int u = i + matrixsize;
					int v = j + matrixsize;

					for (int n = 0; n < 3; n++) {
						if ((y + i) >= 0 && (y + i) < imagein->height && ((x + j) >= 0 && (x + j) < imagein->width)) {
							rgb[n] += (float) ((imagein->pdata[(y + i) * imagein->width + (x + j)] & (0x000000FF << (n * 8))) >> (n * 8)) * matrix[u + v * totalsize];
							weight_sum+=matrix[u + v * totalsize];
							cnt++;
						}
					}
					rgb[3] = (imagein->pdata[y * imagein->width + x] & (0x000000FF << (3 * 8))) >> (3 * 8);
										
					
				}
			}
			for (int n = 0; n < 3; n++)
			{
				//rgb[n] /= weight_sum;
				
				if (rgb[n] > 255) rgb[n] = 255;
				if (rgb[n] < 0) rgb[n] = 0;
			}
			imageout->pdata[(y)*imagein->width + (x)] = 0;
			for (int n = 0; n < 4; n++) {
				imageout->pdata[(y) * imagein->width + (x)] |= (int)rgb[n] << (n * 8);
			}
		}
	}
}


//Verify results
static int Kernel_Verify(pKernelData pdata, pbmp_t testimage) {
	bmp_t image2;
	BmpCopyNew(&image2, &pdata->InputBmp);
	Kernel_CPU(&pdata->InputBmp, &image2, pdata->GaussianKernelSize, pdata->GaussianKernel);

	int result = memcmp(image2.pdata, testimage->pdata, testimage->totalbytes);

	return result;
}

//Run the task using OpenCL
static void Kernel_OpenCL(pKernelData pdata) {

	cl_int status;

	//TODO 6
	//Create the buffers

	//Set kernel arguments

	//Define workitem and workgroup sizes, use 2D, such as 16x16 workitems on width x height global size



	pdata->timeBegin = clock();

	for (int i = 0; i < pdata->RepeatCount; i++) {

		//TODO 6
		//copy data from host to device
		//--copy image data
		//--copy matrix data

		//Enqueue kernel

		//Read back data from device to host


	}

	pdata->timeEnd = clock();

	//Verify results
	int testresult = 0;
	testresult = Kernel_Verify(pdata, &pdata->OutputBmp);
	if (testresult != 0) {
		printf("\r\nTest ERROR!");
	}

	if (testresult == 0) {
		printf("\r\nTest OK!");
	}


	//TODO 6 free up resources


}


//Run the selected task
void KernelBlurRun(int mode, int platformindex, int deviceindex, int repeatcount, int kernelsize, float sigma) {
	//Init Data
	pKernelData pdata = InitKernel(sigma, kernelsize);
	if (pdata == NULL) {
		return;
	}

	pdata->RepeatCount = repeatcount;

	if (mode == MODE_CPU) {
		//Run on CPU
		pdata->timeBegin = clock();
		for (int i = 0; i < pdata->RepeatCount; i++) {
			Kernel_CPU(&pdata->InputBmp, &pdata->OutputBmp, pdata->GaussianKernelSize, pdata->GaussianKernel);
		}
		pdata->timeEnd = clock();
	}

	if (mode == MODE_OPENCL) {
		//Run using OpenCL
		cl_int status;
		status = clInitPlatform(platformindex, deviceindex, CL_DEVICE_TYPE_ALL, "kernel_blur.cl", "blur", NULL, &pdata->OpenCL);
		if (status != 0) {
			printf("Error in opencl init: %d", status);
			FreeKernel(pdata);
			return;
		}

		Kernel_OpenCL(pdata);

		clCleanup(&pdata->OpenCL);
	}

	BmpSave("blur.bmp", &pdata->OutputBmp);

	double time_spent = (double)(pdata->timeEnd - pdata->timeBegin) / CLOCKS_PER_SEC * 1000 / pdata->RepeatCount;

	printf("\r\n %d Iterations, Mean iteration time: %f ms", pdata->RepeatCount, time_spent);

	FreeKernel(pdata);
}
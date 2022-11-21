#include "kernels.h"
#include "bmp.h"

//Descriptor for the task
typedef struct KernelData_ {

	bmp_t InputBmp;
	bmp_t OutputBmp;

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
	pdata->RepeatCount = 1000;

	int err = BmpLoad("raw.bmp", &pdata->InputBmp);

	if (err != 0) {
		printf("Error loading image");
		free(pdata);
		return NULL;
	}
	BmpNew(pdata->InputBmp.width, pdata->InputBmp.height,&pdata->OutputBmp);

	pdata->timeBegin = clock();
	pdata->timeEnd = clock();

	return pdata;
}


//Implement the function in C running on the CPU for verification
static void Kernel_CPU(pbmp_t imagein, pbmp_t imageout) {
	//TODO 5
	//debayer raw pixel data

}

//Verify results
static int Kernel_Verify(pbmp_t inimage, pbmp_t testimage) {
	bmp_t image2;
	BmpCopyNew(&image2, inimage);
	Kernel_CPU(inimage, &image2);

	int result = memcmp(image2.pdata, testimage->pdata, testimage->totalbytes);

	return result;
}

//Run the task using OpenCL
static void Kernel_OpenCL(pKernelData pdata) {

	cl_int status;

	//TODO 5
	//Create the buffers

	//Set kernel arguments
	

	//Define workitem and workgroup sizes
	size_t local_item_size[] = { 16, 16 };
	size_t global_item_size[] = { MakeDivisableUp(pdata->InputBmp.width,local_item_size[0]), MakeDivisableUp(pdata->InputBmp.height,local_item_size[1]) };


	pdata->timeBegin = clock();

	for (int i = 0; i < pdata->RepeatCount; i++) {
		//TODO 5

		//copy data from host to device

		//Enqueue kernel

		//Read back data from device to host


	}

	pdata->timeEnd = clock();


	//Verify results
	int testresult = 0;
	testresult = Kernel_Verify(&pdata->InputBmp, &pdata->OutputBmp);
	if (testresult != 0) {
		printf("\r\nTest ERROR!");
	}

	if (testresult == 0) {
		printf("\r\nTest OK!");
	}



	//TODO 5
	//Free up resources

}


//Run the selected task
void KernelDebayerRun(int mode, int platformindex, int deviceindex, int repeatcount) {

	//Init Data
	pKernelData pdata = InitKernel();
	if (pdata == NULL) {
		return;
	}

	pdata->RepeatCount = repeatcount;

	if (mode == MODE_CPU) {
		//Run on CPU
		pdata->timeBegin = clock();
		for (int i = 0; i < pdata->RepeatCount; i++) {
			Kernel_CPU(&pdata->InputBmp, &pdata->OutputBmp);
		}
		pdata->timeEnd = clock();
	}

	if (mode == MODE_OPENCL) {
		//Run using OpenCL
		cl_int status;
		status = clInitPlatform(platformindex, deviceindex, CL_DEVICE_TYPE_ALL, "kernel_debayer.cl", "debayer", NULL, &pdata->OpenCL);
		if (status != 0) {
			printf("Error in opencl init: %d", status);
			FreeKernel(pdata);
			return;
		}

		Kernel_OpenCL(pdata);
		clCleanup(&pdata->OpenCL);
	}

	BmpSave("debayer.bmp", &pdata->OutputBmp);

	double time_spent = (double)(pdata->timeEnd - pdata->timeBegin) / CLOCKS_PER_SEC * 1000 / pdata->RepeatCount;

	printf("\r\n %d Iterations, Mean iteration time: %f ms", pdata->RepeatCount, time_spent);

	FreeKernel(pdata);
}
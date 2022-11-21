#include "kernels.h"

//Descriptor for the task
typedef struct KernelData_ {

	int BufferSize;
	unsigned int* InputData1;
	unsigned int* InputData2;
	unsigned int* OutputData;
	OpenCL_t OpenCL;
	int RepeatCount;
	clock_t timeBegin;
	clock_t timeEnd;

} KernelData, *pKernelData;


//Free up kernel and OpenCL resources
static void FreeKernel(pKernelData pdata) {
	
	free(pdata->InputData1);
	free(pdata->InputData2);
	free(pdata->OutputData);

	free(pdata);
}

//Initialize task parameters
static pKernelData InitKernel() {
	pKernelData pdata = (pKernelData)malloc(sizeof(struct KernelData_));
	pdata->RepeatCount = 1;

	int buffersize = 10000000;
	pdata->BufferSize = buffersize;
	pdata->InputData1 = (int*)malloc(buffersize * sizeof(int));
	pdata->InputData2 = (int*)malloc(buffersize * sizeof(int));
	pdata->OutputData = (int*)malloc(buffersize * sizeof(int));

	for (int i = 0; i < buffersize; i++) {
		pdata->InputData1[i] = i;
		pdata->InputData2[i] = i;
		pdata->OutputData[i] = 0;
	}

	pdata->timeBegin = clock();
	pdata->timeEnd = clock();
		
	return pdata;
}


//Implement the function in C running on the CPU for verification
static void Kernel_CPU(int buffersize, unsigned int* inbuffer1, unsigned int* inbuffer2, unsigned int* outbuffer) {
	for (int j = 0; j < buffersize; j++) {
		outbuffer[j] = inbuffer1[j] + inbuffer2[j];
	}
}

//Verify results
static int Kernel_Verify(int buffersize, unsigned int* inbuffer1, unsigned int* inbuffer2, unsigned int* outbuffertest ) {
	unsigned int* outbuffer = (unsigned int*)malloc(buffersize * sizeof(unsigned int));
	Kernel_CPU(buffersize, inbuffer1, inbuffer2, outbuffer);

	int result = memcmp(outbuffertest, outbuffer, buffersize);
	
	free(outbuffer);
	return result;
}

//Run the task using OpenCL
static void Kernel_OpenCL(pKernelData pdata) {

	cl_int status;

	//Create the buffers

	cl_mem inputBuffer1 = clCreateBuffer(
		pdata->OpenCL.context,
		CL_MEM_READ_ONLY,//| CL_MEM_COPY_HOST_PTR
		sizeof(cl_int) * pdata->BufferSize,
		NULL,//pdata->InputData1
		&status);

	cl_mem inputBuffer2 = clCreateBuffer(
		pdata->OpenCL.context,
		CL_MEM_READ_ONLY,//| CL_MEM_COPY_HOST_PTR
		sizeof(cl_int) * pdata->BufferSize,
		NULL,//pdata->InputData2
		&status);

	cl_mem outputBuffer = clCreateBuffer(
		pdata->OpenCL.context,
		CL_MEM_WRITE_ONLY,
		sizeof(cl_int) * pdata->BufferSize,
		NULL,//NULL
		&status);

	//Define workitem and workgroup sizes
	size_t global_item_size = pdata->BufferSize; //All items
	size_t local_item_size = 64;// Groups of 64

	size_t maxglobalworkitemsiniteration = pdata->OpenCL.workitem_size[0];

	//Set kernel arguments
	status = clSetKernelArg(pdata->OpenCL.kernel, 0, sizeof(cl_mem), (void*)&inputBuffer1);
	status = clSetKernelArg(pdata->OpenCL.kernel, 1, sizeof(cl_mem), (void*)&inputBuffer2);
	status = clSetKernelArg(pdata->OpenCL.kernel, 2, sizeof(cl_mem), (void*)&outputBuffer);



	pdata->timeBegin = clock();
	for (int i = 0; i < pdata->RepeatCount; i++) {

		//copy data from host to device
		status = clEnqueueWriteBuffer(pdata->OpenCL.commandQueue, inputBuffer1, CL_FALSE, 0,//CL_TRUE
			pdata->BufferSize * sizeof(int), pdata->InputData1, 0, NULL, NULL);
		status = clEnqueueWriteBuffer(pdata->OpenCL.commandQueue, inputBuffer2, CL_FALSE, 0,//CL_TRUE
			pdata->BufferSize * sizeof(int), pdata->InputData2, 0, NULL, NULL);

		clEnqueueBarrierWithWaitList(pdata->OpenCL.commandQueue, 0, 0, 0);


		/*
		//If error when enqueuing large number of workitems
		size_t workitemsdone = 0;
		while (1) {

			size_t wis = global_item_size - workitemsdone;
			if (wis == 0) { break; }
			if (wis > maxglobalworkitemsiniteration) { wis = maxglobalworkitemsiniteration; }
			size_t wg = local_item_size;
			if (wg > wis) { wg = wis; }

			status = clEnqueueNDRangeKernel(pdata->OpenCL.commandQueue, pdata->OpenCL.kernel, 1, &workitemsdone,
				&wis, &wg, 0, NULL, NULL);

			workitemsdone += wis;
		}*/

		//Enqueue kernel
		status = clEnqueueNDRangeKernel(pdata->OpenCL.commandQueue, pdata->OpenCL.kernel, 1, NULL,
			&global_item_size, &local_item_size, 0, NULL, NULL);

		//Read back data from device to host
		status = clEnqueueReadBuffer(pdata->OpenCL.commandQueue, outputBuffer, CL_TRUE, 0,
			pdata->BufferSize * sizeof(int), pdata->OutputData, 0, NULL, NULL);

	}

	pdata->timeEnd = clock();


	//Verify results
	int testresult = 0;
	testresult = Kernel_Verify(pdata->BufferSize, pdata->InputData1, pdata->InputData2, pdata->OutputData);
	if (testresult != 0) {
		printf("\r\nTest ERROR!");
	}

	if (testresult == 0) {
		printf("\r\nTest OK!");
	}


	//Free up resources

	status = clReleaseMemObject(inputBuffer1);
	status = clReleaseMemObject(inputBuffer2);
	status = clReleaseMemObject(outputBuffer);


}


//Run the selected task
void KernelAddRun(int mode, int platformindex, int deviceindex, int repeatcount) {
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
			Kernel_CPU(pdata->BufferSize, pdata->InputData1, pdata->InputData2, pdata->OutputData);
		}
		pdata->timeEnd = clock();
	}			

	if (mode == MODE_OPENCL) {
		//Run using OpenCL
		cl_int status;
		status = clInitPlatform(platformindex, deviceindex, CL_DEVICE_TYPE_ALL, "kernel_add.cl", "vector_add", NULL, &pdata->OpenCL);
		if (status != 0) {
			printf("Error in opencl init: %d", status);
			FreeKernel(pdata);
			return ;
		}
		Kernel_OpenCL(pdata);

		clCleanup(&pdata->OpenCL);
	}

	double time_spent = (double)(pdata->timeEnd - pdata->timeBegin) / CLOCKS_PER_SEC * 1000/pdata->RepeatCount;

	printf("\r\n %d Iterations, Mean iteration time: %f ms", pdata->RepeatCount, time_spent);

	FreeKernel(pdata);
}


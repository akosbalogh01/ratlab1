#include "kernels.h"

//Descriptor for the task
typedef struct KernelData_ {

	int BufferSize;
	unsigned int* InputData;
	unsigned int SumResult;
	OpenCL_t OpenCL;
	int RepeatCount;
	clock_t timeBegin;
	clock_t timeEnd;

} KernelData, * pKernelData;

//Free up kernel and OpenCL resources
static void FreeKernel(pKernelData pdata) {

	free(pdata->InputData);

	free(pdata);
}

//Initialize task parameters
static pKernelData InitKernel() {
	pKernelData pdata = (pKernelData)malloc(sizeof(struct KernelData_));
	pdata->RepeatCount = 1000;

	int buffersize = 1 << 13;// 1 << 14;
	pdata->BufferSize = buffersize;
	pdata->InputData = (int*)malloc(buffersize * sizeof(int));

	for (int i = 0; i < buffersize; i++) {
		pdata->InputData[i] = i;
	}

	pdata->timeBegin = clock();
	pdata->timeEnd = clock();
	
	return pdata;
}


//Implement the function in C running on the CPU for verification
static void Kernel_CPU(int buffersize, unsigned int* inbuffer, unsigned int* sum) {
	//TODO 2
	//sum=sum of inbuffer elements, elements number==buffersize
	if (sum == NULL) {
		return;
	}

	unsigned int* copy = malloc(sizeof(unsigned int) * buffersize);
	memcpy(copy, inbuffer, sizeof(unsigned int) * buffersize);
	unsigned int loopcnt = buffersize / 2;
	while (loopcnt >= 1) {
		for (int i = 0; i < loopcnt; i++) {
			copy[i] += copy[loopcnt + i];
		}

		loopcnt /= 2;
	}

	*sum = copy[0];
}


//Verify results
static int Kernel_Verify(int buffersize, unsigned int* inbuffer, unsigned int sum) {
	unsigned int sum2 = 0;
	Kernel_CPU(buffersize, inbuffer, &sum2);

	int result = sum - sum2;

	return result;
}

//Run the task using OpenCL
static void Kernel_OpenCL(pKernelData pdata) {

	cl_int status;

	//Create the buffers
	cl_mem inputBuffer = clCreateBuffer(
		pdata->OpenCL.context,
		CL_MEM_READ_ONLY,//| CL_MEM_COPY_HOST_PTR
		sizeof(cl_int) * pdata->BufferSize,
		NULL,//pdata->InputData
		&status);

	//Define workitem and workgroup sizes
	size_t local_item_size = 64;// Groups of 64
	size_t global_item_size = MakeDivisableUp(pdata->BufferSize, local_item_size); //All workitems

	int groupsnum = global_item_size / local_item_size;

	unsigned int* results = (unsigned int*)malloc(groupsnum * sizeof(unsigned int));


	cl_mem clresults = clCreateBuffer(
		pdata->OpenCL.context,
		CL_MEM_WRITE_ONLY,
		sizeof(cl_int) * groupsnum,
		NULL,//NULL
		&status);

	//Set kernel arguments
	status = clSetKernelArg(pdata->OpenCL.kernel, 0, sizeof(cl_mem), (void*)&inputBuffer);
	status = clSetKernelArg(pdata->OpenCL.kernel, 1, sizeof(cl_mem), (void*)&clresults);
	status = clSetKernelArg(pdata->OpenCL.kernel, 2, sizeof(cl_int), (void*)&pdata->BufferSize);



	pdata->timeBegin = clock();

	for (int i = 0; i < pdata->RepeatCount; i++) {

		//TODO 2

		//copy data from host to device

		//Enqueue kernel

		//Read back data from device to host


		//Finalize sums

		unsigned int sum = 0;
		for (int j = 0; j < groupsnum; j++) {
			unsigned int r = results[j];
			sum += r;
		}

		pdata->SumResult = sum;

	}

	pdata->timeEnd = clock();


	//Verify results
	int testresult = 0;
	testresult = Kernel_Verify(pdata->BufferSize, pdata->InputData, pdata->SumResult);
	if (testresult != 0) {
		printf("\r\nTest ERROR!");
	}

	if (testresult == 0) {
		printf("\r\nTest OK!");
	}



	free(results);

	//Free up resources
	status = clReleaseMemObject(inputBuffer);
	status = clReleaseMemObject(clresults);

}
 

//Run the selected task
void KernelSumReduceRun(int mode, int platformindex, int deviceindex, int repeatcount) {

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
			Kernel_CPU(pdata->BufferSize, pdata->InputData, &pdata->SumResult);
		}
		pdata->timeEnd = clock();
	}

	if (mode == MODE_OPENCL) {
		//Run using OpenCL
		cl_int status;
		status = clInitPlatform(platformindex, deviceindex, CL_DEVICE_TYPE_ALL, "kernel_sumreduce.cl", "sumreduce", NULL, &pdata->OpenCL);
		if (status != 0) {
			printf("Error in opencl init: %d", status);
			FreeKernel(pdata);
			return;
		}

		Kernel_OpenCL(pdata);

		clCleanup(&pdata->OpenCL);


	}

	double time_spent = (double)(pdata->timeEnd - pdata->timeBegin) / CLOCKS_PER_SEC * 1000 / pdata->RepeatCount;

	printf("\r\n %d Iterations, Mean iteration time: %f ms", pdata->RepeatCount, time_spent);

	FreeKernel(pdata);
}
//imagein: Pointer for image data
//width: width of the image
//height: height of the image
//matrix: pointer to kernel matrix data
//matrixsize: matrix size (matrixsize*2+1)x(matrixsize*2+1)
//imageout: output image data pointer

__kernel void blur(__global const int *imagein, const int width, const int height, __global const float* matrix, const int matrixsize, __global int *imageout) {
   
	//TODO 6
	//implement convolution for each pixel
	unsigned int y=get_global_id(0);
	unsigned int x=get_global_id(1);
	int totalsize = (matrixsize * 2 + 1);

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
				if ((y + i) >= 0 && (y + i) < height && ((x + j) >= 0 && (x + j) < width)) {
					rgb[n] += (float) ((imagein[(y + i) * width + (x + j)] & (0x000000FF << (n * 8))) >> (n * 8)) * matrix[u + v * totalsize];
					weight_sum+=matrix[u + v * totalsize];
					cnt++;
				}
			}
			rgb[3] = (imagein[y * width + x] & (0x000000FF << (3 * 8))) >> (3 * 8);
										
					
		}
	}
	for (int n = 0; n < 3; n++)
	{
		// rgb[n] /= weight_sum;
				
		if (rgb[n] > 255) rgb[n] = 255;
		if (rgb[n] < 0) rgb[n] = 0;
	}
	imageout[(y)*width + (x)] = 0;
	for (int n = 0; n < 4; n++) {
		imageout[(y) * width + (x)] |= (int)rgb[n] << (n * 8);
	}
}

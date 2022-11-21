//image: input/output image data pointer
//brightness: brightness value
//pixelsNum: number of pixels

__kernel void brightness(__global int *image, const unsigned char brightness, const int pixelsNum) {
    //TODO 3
    //Set brightness of image pixels

    __local int rgb[4];

	int i = get_global_id(0);

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

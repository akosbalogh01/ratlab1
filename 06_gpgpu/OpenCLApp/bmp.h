#pragma once

//Bitmap descriptor struct
typedef struct _bmp {
	unsigned int* pdata;
	int width;
	int height;
	int totalbytes;
} bmp_t, *pbmp_t;

//Load a bitmap from file   
int BmpLoad(const char* filename, pbmp_t pbmp);

//Save a bitmap to file
int BmpSave(const char* filename, pbmp_t pbmp);

//Close bitmap, free resources
void BmpClose(pbmp_t pbmp);

//Create a new bitmap in memory
void BmpNew(int width, int height, pbmp_t pbmp);

//Copy a bitmap
void BmpCopy(pbmp_t pbmp2, pbmp_t pbmp);

//Create a new copy of a bitmap
void BmpCopyNew(pbmp_t pbmp2, pbmp_t pbmp);
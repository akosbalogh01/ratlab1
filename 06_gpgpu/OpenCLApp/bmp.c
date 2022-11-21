#include "bmp.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#pragma pack(push,1)

typedef struct {
    char         filetype[2];   /* magic - always 'B' 'M' */
    unsigned int filesize;
    short        reserved1;
    short        reserved2;
    unsigned int dataoffset;    /* offset in bytes to actual bitmap data */
} file_header;

typedef struct {
    file_header  fileheader;
    unsigned int headersize;
    int          width;
    int          height;
    short        planes;
    short        bitsperpixel;  /* we only support the value 24 here */
    unsigned int compression;   /* we do not support compression */
    unsigned int bitmapsize;
    int          horizontalres;
    int          verticalres;
    unsigned int numcolors;
    unsigned int importantcolors;
} bitmap_header;

#pragma pack(pop)

void BmpNew(int width, int height, pbmp_t pbmp) {
    pbmp->width = width;
    pbmp->height = height;
    pbmp->totalbytes = width * height * 4;
    pbmp->pdata = (unsigned int*)malloc(width * height * sizeof(int));

}

void BmpCopyNew(pbmp_t pbmp2, pbmp_t pbmp) {
    BmpNew(pbmp->width, pbmp->height, pbmp2);
    memcpy(pbmp2->pdata, pbmp->pdata, pbmp->width * pbmp->height*4);
}

void BmpCopy(pbmp_t pbmp2, pbmp_t pbmp) {
    if (pbmp2->height != pbmp->height || pbmp2->width != pbmp->width) {
        free(pbmp->pdata);
        BmpCopyNew(pbmp2, pbmp);
        return;
    }
    memcpy(pbmp2->pdata, pbmp->pdata, pbmp->width * pbmp->height*4);
}


int BmpLoad(const char* filename, pbmp_t pbmp) {

    FILE* fp;
    bitmap_header hp;

    //Open input file:
    if (fopen_s(&fp,filename, "rb")!=0){
        printf("\r\nError opening file %s", filename);
        return 1;
    }

    int n = fread(&hp, 1,sizeof(bitmap_header), fp);
    if (n != sizeof(bitmap_header)) {
        fclose(fp);
        printf("\r\nError reading header");
        return 2;
    }

    if (hp.bitsperpixel == 32 || hp.bitsperpixel == 24 || hp.bitsperpixel==8) {
    }else{
        fclose(fp);
        printf("\r\nError unknow bits/pixel in header");
        return 3;
    }

    fseek(fp, hp.fileheader.dataoffset, SEEK_SET);
    int stride = hp.width * 4;

    unsigned char* row = NULL;
    if (hp.bitsperpixel == 24) {
        stride = (hp.width * 3 + 2) & 0xfffffffc;
        row = (unsigned char*)malloc(stride);
        if (row == NULL) {
            printf("\r\nError allocating memory for image row");
            fclose(fp);
            return 4;
        }
    }
    if (hp.bitsperpixel == 8) {
        stride = (hp.width * 4 + 3) & 0xfffffffc;
        row = (unsigned char*)malloc(stride);
        if (row == NULL) {
            fclose(fp);
            printf("\r\nError allocating memory for image row");
            return 5;
        }
    }

    pbmp->totalbytes =hp.width * hp.height * 4;

    unsigned int* data = (unsigned int*)malloc(pbmp->totalbytes);

    unsigned int* dataptr;
    for (int i = 0; i < hp.height; i++) {
        dataptr = data + (hp.height - 1-i) * hp.width;
        n = 0;
        if (hp.bitsperpixel == 32) {
            n = fread(dataptr, 1,stride,  fp);
        }
        else if (hp.bitsperpixel == 24) {
            n = fread(row, 1,stride, fp);
            for (int j = 0; j < hp.width; j++) {
                dataptr[j] = row[j * 3] | (row[j * 3 + 1] << 8) | (row[j * 3 + 2] << 16) | 0xff000000;
            }
        }
        else if (hp.bitsperpixel == 8) {
            //8 bpp
            n = fread(row, 1, stride, fp);
            for (int j = 0; j < hp.width; j++) {
                dataptr[j] = row[j] | (row[j] << 8) | (row[j] << 16) | 0xff000000;
            }
        }


        if (n != stride) {
            free(data);
            fclose(fp);
            printf("\r\nError reading image row");
            return 6;
        }
    }
    fclose(fp);
    pbmp->pdata = data;
    pbmp->width = hp.width;
    pbmp->height = hp.height;
    return 0;
}


int BmpSave(const char* filename, pbmp_t pbmp) {

    FILE* fp;
    bitmap_header hp;

    //Open input file:
    if (fopen_s(&fp, filename, "wb") != 0) {
        printf("\r\nError opening file in write mode %s", filename);
        return 1;
    }
    hp.fileheader.filetype[0] = 'B';
    hp.fileheader.filetype[1] = 'M';
    hp.fileheader.dataoffset = sizeof(bitmap_header);
    hp.fileheader.filesize = pbmp->width * pbmp->height * 4 + sizeof(bitmap_header);
    hp.fileheader.reserved1 = 0;
    hp.fileheader.reserved2 = 0;
    hp.bitmapsize = pbmp->width * pbmp->height*4;
    hp.bitsperpixel = 32;
    hp.compression = 0;
    hp.fileheader.dataoffset = sizeof(bitmap_header);
    hp.headersize= sizeof(bitmap_header)-sizeof(file_header);
    hp.height = pbmp->height;
    hp.planes = 1;
    hp.width = pbmp->width;
    hp.horizontalres = 0;
    hp.verticalres = 0;
    hp.importantcolors = 0;
    hp.numcolors = 0;

    fwrite(&hp, 1, sizeof(bitmap_header), fp);

    int stride = hp.width * 4;

    unsigned int* dataptr;
    int n;
    for (int i = 0; i < hp.height; i++) {
        dataptr = pbmp->pdata + (hp.height - 1 - i) * hp.width;
        n = fwrite(dataptr, 1, stride, fp);

        if (n != stride) {
            fclose(fp);
            printf("\r\nError writing image row");
            return 2;
        }
    }

    fclose(fp);
    return 0;
}
void BmpClose(pbmp_t pbmp) {
    if (pbmp != NULL) {
        free(pbmp->pdata);
    }
}
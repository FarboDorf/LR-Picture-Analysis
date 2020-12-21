#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

typedef struct
{
    unsigned int    bfType;
    unsigned long   bfSize;
    unsigned int    bfReserved1;
    unsigned int    bfReserved2;
    unsigned long   bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
    unsigned int    biSize;
    int             biWidth;
    int             biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    int             biXPelsPerMeter;
    int             biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
    int   rgbBlue;
    int   rgbGreen;
    int   rgbRed;
    int   rgbReserved;
} RGBQUAD;

typedef struct
{
    int   width;
    int   height;
    RGBQUAD **rgb;
} PICTURE;

static unsigned short read_u16(FILE* fp);
static unsigned int   read_u32(FILE* fp);
static int            read_s32(FILE* fp);


PICTURE getPic(FILE* pFile){
	PICTURE pic;

	BITMAPFILEHEADER header;

    header.bfType = read_u16(pFile);
    header.bfSize = read_u32(pFile);
    header.bfReserved1 = read_u16(pFile);
    header.bfReserved2 = read_u16(pFile);
    header.bfOffBits = read_u32(pFile);

    //printf("%x %d %d\n", header.bfType, header.bfSize, header.bfOffBits);
    //system("pause");
    BITMAPINFOHEADER bmiHeader;

    bmiHeader.biSize = read_u32(pFile);
    bmiHeader.biWidth = read_s32(pFile);
    bmiHeader.biHeight = read_s32(pFile);
    bmiHeader.biPlanes = read_u16(pFile);
    bmiHeader.biBitCount = read_u16(pFile);
    bmiHeader.biCompression = read_u32(pFile);
    bmiHeader.biSizeImage = read_u32(pFile);
    bmiHeader.biXPelsPerMeter = read_s32(pFile);
    bmiHeader.biYPelsPerMeter = read_s32(pFile);
    bmiHeader.biClrUsed = read_u32(pFile);
    bmiHeader.biClrImportant = read_u32(pFile);
    //printf("width\theight\n%d\t%d\n", bmiHeader.biWidth, bmiHeader.biHeight);
    //printf("bits per pixel\n%d\n", bmiHeader.biBitCount);
    //printf("biCompression\n%d\n", bmiHeader.biCompression);

    //cout << bmiHeader.biSize << ' ' << bmiHeader.biWidth << ' ' << bmiHeader.biHeight << endl;
	//system("pause");

    RGBQUAD** rgb = new RGBQUAD * [bmiHeader.biWidth];
    for (int i = 0; i < bmiHeader.biWidth; i++) {
        rgb[i] = new RGBQUAD[bmiHeader.biHeight];
    }

    for (int i = 0; i < bmiHeader.biWidth; i++) {
        for (int j = 0; j < bmiHeader.biHeight; j++) {
            rgb[i][j].rgbBlue = getc(pFile);
            rgb[i][j].rgbGreen = getc(pFile);
            rgb[i][j].rgbRed = getc(pFile);

        }
        getc(pFile);
    }
    fclose(pFile);

    pic.width = bmiHeader.biWidth;
    pic.height = bmiHeader.biHeight;
    pic.rgb = rgb;

    return pic;
}

int sort(int color){
	if(color < 64) return 0;
	if(color < 128) return 1;
	if(color < 192) return 2;
	if(color < 256) return 3;
	else cout << "!!!!!!" << color << endl;
}

int* histogramm(PICTURE pic){
	int index = 0;
	int rDip;
	int gDip;
	int bDip;
	int *gist = (int*)calloc(64, sizeof(int));
	for (int i = 0; i < pic.width; i++) {
        for (int j = 0; j < pic.height; j++) {
        	rDip = sort(pic.rgb[i][j].rgbRed);
        	gDip = sort(pic.rgb[i][j].rgbGreen) << 2;
        	bDip = sort(pic.rgb[i][j].rgbBlue) << 4;
        	index = rDip + gDip + bDip;
			gist[index] += 1;
    	}
	}
	return gist;
}

double sqrtDist(int *arr1, int *arr2){
	double sum = 0;
	for(int i=0; i < 64; i++){
		sum += (arr1[i] - arr2[i]) * (arr1[i] - arr2[i]);
	}
	return sqrt(sum);
}

double sqrtDist(double *arr1, double *arr2){
	double sum = 0;
	for(int i=0; i < 64; i++){
		sum += (arr1[i] - arr2[i]) * (arr1[i] - arr2[i]);
	}
	return sqrt(sum);
}

double* absToReg(int* arr){
	double *RegGist = (double*)calloc(64, sizeof(double));
	int max = arr[0];
	for(int i=0; i < 64; i++){
		if(arr[i] > max) max = arr[i];
	}
	for(int i=0; i < 64; i++){
		RegGist[i] = (double)arr[i] / max;
	}
	return RegGist;
}

int main()
{
	char path[4][255] = {
		"C:/Users/Пользователь/Desktop/MAI/II Курс/Прога/ПЗ/Анализ картинок/cat_grey.bmp",
		"C:/Users/Пользователь/Desktop/MAI/II Курс/Прога/ПЗ/Анализ картинок/mouse.bmp"
	};
	PICTURE pic[4];
	int *absGist[4];
	double *regGist[4];
	double dist[4][4] = {0};

	for(int i=0; i < 4; i++){
		//cout << path[i][] << endl;
		FILE* imgF = fopen(path[i], "rb");
    	pic[i] = getPic(imgF);
    	cout << i + 1 << " Width " << pic[i].width << " Height " << pic[i].height << endl;
		absGist[i] = histogramm(pic[i]);
		/*
		for(int j = 0; j < 64; j++) {
        	cout << gist[i][j] << endl;
		}*/
	}

	for(int i=0l; i < 4; i++){
		regGist[i] = absToReg(absGist[i]);
	}
	/*
	for(int i=0; i < 4; i++){
		for(int j=0; j < 4; j++){
			dist[i][j] = sqrtDist(absGist[i], absGist[j]);
			//cout << fixed << setprecision(3) << dist[i][j] << "\t";
			cout << dist[i][j] << "\t";
		}
		cout << "\n";
	}*/

	cout << "\n\n";

	for(int i=0; i < 4; i++){
		for(int j=0; j < 4; j++){
			dist[i][j] = sqrtDist(regGist[i], regGist[j]);
			cout << dist[i][j] << "\t";
		}
		cout << "\n";
	}


    for(int i = 0; i < 4; i++){
    	delete pic[i].rgb;
	}
    return 0;
}


static unsigned short read_u16(FILE* fp)
{
    unsigned char b0, b1;

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 << 8) | b0);
}


static unsigned int read_u32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


static int read_s32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

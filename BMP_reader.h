/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/


#ifndef BMP_READER
#define BMP_READER

#include <iostream>
#include <fstream>

#include "cstring"
#include "string"
#include <stdio.h>

typedef int FXPT2DOT30;

typedef struct
{
    FXPT2DOT30 ciexyzX;
    FXPT2DOT30 ciexyzY;
    FXPT2DOT30 ciexyzZ;
} CIEXYZ;

typedef struct
{
    CIEXYZ  ciexyzRed;
    CIEXYZ  ciexyzGreen;
    CIEXYZ  ciexyzBlue;
} CIEXYZTRIPLE;

// bitmap file header
typedef struct
{
    unsigned short bfType;
    unsigned int   bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int   bfOffBits;
} BITMAPFILEHEADER;

// bitmap info header
typedef struct
{
    unsigned int   biSize;
    unsigned int   biWidth;
    unsigned int   biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int   biCompression;
    unsigned int   biSizeImage;
    unsigned int   biXPelsPerMeter;
    unsigned int   biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
    unsigned int   biRedMask;
    unsigned int   biGreenMask;
    unsigned int   biBlueMask;
    unsigned int   biAlphaMask;
    unsigned int   biCSType;
    CIEXYZTRIPLE   biEndpoints;
    unsigned int   biGammaRed;
    unsigned int   biGammaGreen;
    unsigned int   biGammaBlue;
    unsigned int   biIntent;
    unsigned int   biProfileData;
    unsigned int   biProfileSize;
    unsigned int   biReserved;
} BITMAPINFOHEADER;

// rgb quad
typedef struct
{
    unsigned char  rgbBlue;
    unsigned char  rgbGreen;
    unsigned char  rgbRed;
    unsigned char  rgbReserved;
} RGBQUAD;

class ImD
{
public:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    RGBQUAD** pixels;
    int padding;

    double gaussianKernel[3][3] =
    {
        {0.0585, 0.0965, 0.0585},
        {0.0965, 0.1592, 0.0965},
        {0.0585, 0.0965, 0.0585}
    };




    ImD(const BITMAPFILEHEADER& fh,const BITMAPINFOHEADER& ih, RGBQUAD** pixelData, int pad): fileHeader(fh),infoHeader(ih),padding(pad)
    {

        pixels = new RGBQUAD*[infoHeader.biHeight];
        for (unsigned int i=0; i<infoHeader.biHeight; ++i )
        {
            pixels[i] = new RGBQUAD[infoHeader.biWidth];
            for (unsigned int j=0; j<infoHeader.biWidth; ++j)
            {
                pixels[i][j] = pixelData[i][j];
            }
        }
    }

    ~ImD()
    {
        for (unsigned int i=0; i<infoHeader.biHeight; ++i)
        {
            delete [] pixels[i];
        }
        delete[] pixels;
    }

    void applyGaussianFilter(ImD*);


    void rotate90ContClockwise(std::string);

    void rotate90Clockwise(std::string,bool);






};







// read bytes
template <typename Type>
void read(std::ifstream &fp, Type &result, std::size_t size)
{
    fp.read(reinterpret_cast<char*>(&result), size);
}


int reader(std::string,bool,bool,bool);

#endif
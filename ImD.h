/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/
#ifndef ImD_class
#define ImD_class

#include <vector>
#include <fstream>
#include <iostream>

#include <cstring>
#include <string>
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
} RGBQUAD;

class ImD
{
private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    RGBQUAD* pixels; // Одномерный массив пикселей
    int padding;

    double gaussianKernel[3][3] =
    {
        {0.0585, 0.0965, 0.0585},
        {0.0965, 0.1592, 0.0965},
        {0.0585, 0.0965, 0.0585}
    };

public:

    ImD(const ImD& image);
    ImD();
    ImD(const BITMAPFILEHEADER& fh, const BITMAPINFOHEADER& ih, RGBQUAD* pixelData, int pad);
    ~ImD();

    void applyGaussianFilter(const std::string& fileName);
    void writeBMP(const std::string& fileName,const ImD& image);
    void rotate90ContClockwise(const std::string rFileName);
    void rotate90Clockwise(const std::string rFileName, bool needGaus);
    void reader(std::string fileName, bool need_clock, bool need_contrclock, bool need_Gaus);

    template <typename Type>
    void read(std::ifstream &fp, Type &result, std::size_t size);

    BITMAPINFOHEADER getInfoHeader();
    BITMAPFILEHEADER getFileHeader();
    void setHeight(unsigned int h);
    void setWidth(unsigned int w);
    unsigned int getWidth();

    void setPixels_null();
    int getPadding();
    void setPixel_1(unsigned int cord,RGBQUAD Data);
    void setPixel_all(RGBQUAD* Data);
    RGBQUAD* getPixels_all();


    int getIndex(int x, int y) const
    {
        return y * infoHeader.biWidth + x;
    }
};

#endif
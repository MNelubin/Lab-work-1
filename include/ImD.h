/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/
#ifndef ImD_class
#define ImD_class

#include <vector>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

struct BITMAPFILEHEADER
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BITMAPINFOHEADER
{
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
    uint32_t biRedMask;
    uint32_t biGreenMask;
    uint32_t biBlueMask;
    uint32_t biAlphaMask;
    uint32_t biCSType;
    int32_t  biEndpoints[9];
    uint32_t biGammaRed;
    uint32_t biGammaGreen;
    uint32_t biGammaBlue;
    uint32_t biIntent;
    uint32_t biProfileData;
    uint32_t biProfileSize;
    uint32_t biReserved;
};

struct RGBQUAD
{
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    // unsigned char rgbReserved;
};

class ImD
{
private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    RGBQUAD* pixels;
    int padding;
    const double gaussianKernel[3][3] =
    {
        {1.0 / 16, 2.0 / 16, 1.0 / 16},
        {2.0 / 16, 4.0 / 16, 2.0 / 16},
        {1.0 / 16, 2.0 / 16, 1.0 / 16}
    };

public:
    ImD();
    ImD(const ImD& image);
    ImD(const BITMAPFILEHEADER& fh, const BITMAPINFOHEADER& ih, RGBQUAD* pixelData, int pad);
    ~ImD();

    void reader(std::string fileName, bool need_clock, bool need_contrclock, bool need_Gaus);
    void writeBMP(const std::string& fileName, const ImD& imageData);

    void setHeight(unsigned int h);
    void setWidth(unsigned int w);
    void setPixel_1(unsigned int cord, RGBQUAD Data);
    void setPixel_all(RGBQUAD* Data);
    void setPixels_null();

    unsigned int getWidth();
    unsigned int getHeight();
    unsigned int getIndex(unsigned int x, unsigned int y) const;
    int getPadding();
    RGBQUAD* getPixels_all();
    BITMAPINFOHEADER getInfoHeader();
    BITMAPFILEHEADER getFileHeader();

    void rotate90Clockwise(const std::string rFileName, bool WGaus = false);
    void rotate90ContClockwise(const std::string rFileName);
    void applyGaussianFilter(const std::string& fileName);

    void rotate90Clockwise_parallel(const std::string rFileName, bool WGaus = false);
    void rotate90ContClockwise_parallel(const std::string rFileName);
    void applyGaussianFilter_parallel(const std::string& fileName);

    template <typename Type>
    void read(std::ifstream &fp, Type &result, std::size_t size);

    // Copy assignment operator
    ImD& operator=(const ImD& other);
#endif
};
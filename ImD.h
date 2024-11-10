
#ifndef ImD_class
#define ImD_class

#include "vector"
#include <fstream>
#include "BMP_reader.h"


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




#endif
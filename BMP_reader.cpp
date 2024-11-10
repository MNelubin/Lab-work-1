/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/


#include "BMP_reader.h"
#include "BMP_writer.h"
#include "ImD.h"



int reader(std::string fileName,bool need_clock,bool need_contrclock,bool need_Gaus)
{




    std::ifstream fil_r(fileName, std::ifstream::binary);

    BITMAPFILEHEADER fileHeader;
    read(fil_r, fileHeader.bfType, sizeof(fileHeader.bfType));
    read(fil_r, fileHeader.bfSize, sizeof(fileHeader.bfSize));
    read(fil_r, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
    read(fil_r, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
    read(fil_r, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));


    BITMAPINFOHEADER fileInfoHeader;
    read(fil_r, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));


    if (fileInfoHeader.biSize >= 12)
    {
        read(fil_r, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
        read(fil_r, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
        read(fil_r, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
        read(fil_r, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
    }


    int colorsCount = fileInfoHeader.biBitCount >> 3;
    if (colorsCount < 3)
    {
        colorsCount = 3;
    }

    int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
    int maskValue = (1 << bitsOnColor) - 1;


    if (fileInfoHeader.biSize >= 40)
    {
        read(fil_r, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
        read(fil_r, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
        read(fil_r, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));
        read(fil_r, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));
        read(fil_r, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
        read(fil_r, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
    }


    fileInfoHeader.biRedMask = 0;
    fileInfoHeader.biGreenMask = 0;
    fileInfoHeader.biBlueMask = 0;

    if (fileInfoHeader.biSize >= 52)
    {
        read(fil_r, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
        read(fil_r, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
        read(fil_r, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
    }

    if (fileInfoHeader.biSize >= 56)
    {
        read(fil_r, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
    }
    else
    {
        fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
    }

    if (fileInfoHeader.biSize >= 108)
    {
        read(fil_r, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
        read(fil_r, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
        read(fil_r, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
        read(fil_r, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
        read(fil_r, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
    }


    if (fileInfoHeader.biSize >= 124)
    {
        read(fil_r, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
        read(fil_r, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
        read(fil_r, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
        read(fil_r, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
    }


    int linePadding = (4 - (fileInfoHeader.biWidth * 3) % 4) % 4;

    RGBQUAD** rgbInfo = new RGBQUAD*[fileInfoHeader.biHeight];
    for (int i = 0; i < fileInfoHeader.biHeight; ++i)
    {
        rgbInfo[i] = new RGBQUAD[fileInfoHeader.biWidth];
    }

    for (int i = 0; i < fileInfoHeader.biHeight; ++i)
    {
        for (int j = 0; j < fileInfoHeader.biWidth; ++j)
        {
            fil_r.read(reinterpret_cast<char*>(&rgbInfo[i][j].rgbBlue), sizeof(unsigned char));
            fil_r.read(reinterpret_cast<char*>(&rgbInfo[i][j].rgbGreen), sizeof(unsigned char));
            fil_r.read(reinterpret_cast<char*>(&rgbInfo[i][j].rgbRed), sizeof(unsigned char));
        }
        fil_r.ignore(linePadding);
    }

    ImD* orig = new ImD(fileHeader,fileInfoHeader,rgbInfo,linePadding);


    for (unsigned int i=0; i<fileInfoHeader.biHeight; ++i)
    {
        delete [] rgbInfo[i];
    }
    delete[] rgbInfo;


    if (need_contrclock)
    {
        orig->rotate90ContClockwise(fileName+"_rotContC.bmp");
    }
    if (need_clock)
    {
        if (need_Gaus)
        {
            orig->rotate90Clockwise(fileName+"_rotC.bmp",1);
        }
        else
        {
            orig->rotate90Clockwise(fileName+"_rotC.bmp",0);
        }
    }


    return 1;
}
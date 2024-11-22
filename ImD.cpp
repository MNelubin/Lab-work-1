#include "ImD.h"


template <typename Type>
void ImD::read(std::ifstream &fp, Type &result, std::size_t size)
{
    fp.read(reinterpret_cast<char*>(&result), size);
}

ImD::ImD(const ImD& image) : fileHeader(image.fileHeader), infoHeader(image.infoHeader), padding(image.padding)
{
    pixels = new RGBQUAD[infoHeader.biHeight * infoHeader.biWidth];
    for (unsigned int i = 0; i < infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < infoHeader.biWidth; ++j)
        {
            pixels[getIndex(j, i)] = image.pixels[image.getIndex(j, i)];
        }
    }
}


ImD::ImD()
    : fileHeader(), infoHeader(), pixels(nullptr), padding(0)
{
}

ImD::ImD(const BITMAPFILEHEADER& fh, const BITMAPINFOHEADER& ih, RGBQUAD* pixelData, int pad)
    : fileHeader(fh), infoHeader(ih), padding(pad)
{
    pixels = new RGBQUAD[infoHeader.biHeight * infoHeader.biWidth];
    for (unsigned int i = 0; i < infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < infoHeader.biWidth; ++j)
        {
            pixels[getIndex(j, i)] = pixelData[getIndex(j, i)];
        }
    }
}

ImD::~ImD()
{
    delete[] pixels;
}


void ImD::writeBMP(const std::string& fileName, const ImD& imageData)
{
    std::ofstream file(fileName, std::ios::binary);
    if (!file)
    {
        std::cerr << "Ошибка при создании файла: " << fileName << std::endl;
    }

    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfType), sizeof(imageData.fileHeader.bfType));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfSize), sizeof(imageData.fileHeader.bfSize));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfReserved1), sizeof(imageData.fileHeader.bfReserved1));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfReserved2), sizeof(imageData.fileHeader.bfReserved2));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfOffBits), sizeof(imageData.fileHeader.bfOffBits));

    file.write(reinterpret_cast<const char*>(&imageData.infoHeader), (int)imageData.infoHeader.biSize);

    int linePadding = (4 - (imageData.infoHeader.biWidth * 3) % 4) % 4;
    unsigned char padding[3] = {0, 0, 0}; // Буфер для записи отступа

    for (int i = 0; i < imageData.infoHeader.biHeight; ++i)
    {
        for (int j = 0; j < imageData.infoHeader.biWidth; ++j)
        {
            file.write(reinterpret_cast<const char*>(&imageData.pixels[getIndex(j, i)].rgbBlue), sizeof(unsigned char));
            file.write(reinterpret_cast<const char*>(&imageData.pixels[getIndex(j, i)].rgbGreen), sizeof(unsigned char));
            file.write(reinterpret_cast<const char*>(&imageData.pixels[getIndex(j, i)].rgbRed), sizeof(unsigned char));
        }
        file.write(reinterpret_cast<const char*>(padding), linePadding);
    }

    file.close();
    std::cout << "Файл успешно записан: " << fileName << std::endl;
}


void ImD::setHeight(unsigned int h)
{
    this->infoHeader.biHeight =h;
}
void ImD::setWidth(unsigned int w)
{
    this->infoHeader.biWidth=w;
}
void ImD::setPixel_1(unsigned int cord,RGBQUAD Data)
{
    pixels[cord]=Data;
}
unsigned int ImD::getWidth()
{
    return infoHeader.biWidth;
}





void ImD::rotate90ContClockwise(const std::string rFileName)
{
    ImD* rotatedImage= new ImD(*this);

    rotatedImage->setWidth(infoHeader.biHeight);
    rotatedImage->setHeight(infoHeader.biWidth);

    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            int srcindx = i*infoHeader.biWidth+j;
            int dest =(infoHeader.biWidth-j-1)*infoHeader.biHeight+i;
            rotatedImage->setPixel_1(dest,pixels[srcindx]);
        }

    }

    rotatedImage->writeBMP(rFileName,*rotatedImage);

    delete rotatedImage;
}


void ImD::rotate90Clockwise(const std::string rFileName, bool WGaus)
{

    ImD* rotatedImage= new ImD(*this);


    rotatedImage->setWidth(infoHeader.biHeight);
    rotatedImage->setHeight(infoHeader.biWidth);


    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            rotatedImage->setPixel_1(rotatedImage->getIndex(rotatedImage->getWidth()-1-i,j),pixels[getIndex(j,i)]);
        }
    }

    rotatedImage->writeBMP(rFileName,*rotatedImage);

    if (WGaus)
    {
        infoHeader=rotatedImage->getInfoHeader();

        fileHeader=rotatedImage->getFileHeader();

        delete[] pixels;

        pixels=rotatedImage->getPixels_all();

        rotatedImage->setPixels_null();

        padding=rotatedImage->getPadding();

    }
    else
    {
    }
    delete rotatedImage;
}

void ImD::setPixels_null()
{
    pixels=nullptr;
}
int ImD::getPadding()
{
    return padding;
}
void ImD::setPixel_all(RGBQUAD* Data)
{
    pixels=Data;
}
RGBQUAD* ImD::getPixels_all()
{
    return pixels;
}



void ImD::applyGaussianFilter(const std::string& fileName)
{
    RGBQUAD* newPixels = new RGBQUAD[infoHeader.biHeight * infoHeader.biWidth];

    for (unsigned int y = 1; y < infoHeader.biHeight - 1; ++y)
    {
        for (unsigned int x = 1; x < infoHeader.biWidth - 1; ++x)
        {
            double red = 0.0, green = 0.0, blue = 0.0;

            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int pixelY = y + ky;
                    int pixelX = x + kx;

                    RGBQUAD currentPixel = pixels[getIndex(pixelX, pixelY)];

                    red += currentPixel.rgbRed * gaussianKernel[ky + 1][kx + 1];
                    green += currentPixel.rgbGreen * gaussianKernel[ky + 1][kx + 1];
                    blue += currentPixel.rgbBlue * gaussianKernel[ky + 1][kx + 1];
                }
            }

            newPixels[getIndex(x, y)] = {static_cast<unsigned char>(blue), static_cast<unsigned char>(green), static_cast<unsigned char>(red)};
        }
    }

    // Обработка границ изображения
    for (unsigned int i = 0; i < infoHeader.biHeight; ++i)
    {
        newPixels[getIndex(0, i)] = newPixels[getIndex(1, i)];
        newPixels[getIndex(infoHeader.biWidth - 1, i)] = newPixels[getIndex(infoHeader.biWidth - 2, i)];
    }

    for (unsigned int i = 0; i < infoHeader.biWidth; ++i)
    {
        newPixels[getIndex(i, 0)] = newPixels[getIndex(i, 1)];
        newPixels[getIndex(i, infoHeader.biHeight - 1)] = newPixels[getIndex(i, infoHeader.biHeight - 2)];
    }

    delete[] pixels;
    pixels = newPixels;
    writeBMP(fileName+"_WGaus.bmp",*this);
}

void ImD::reader(std::string fileName, bool need_clock, bool need_contrclock, bool need_Gaus)
{
    if (pixels)
    {
        delete[] pixels;
        pixels =nullptr;
    }
    std::ifstream fil_r(fileName, std::ifstream::binary);

    if (!fil_r)
    {
        std::cerr << "Ошибка при открытии файла: " << fileName << std::endl;
        return;
    }

    read(fil_r, fileHeader.bfType, sizeof(fileHeader.bfType));
    read(fil_r, fileHeader.bfSize, sizeof(fileHeader.bfSize));
    read(fil_r, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
    read(fil_r, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
    read(fil_r, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));

    read(fil_r, infoHeader.biSize, sizeof(infoHeader.biSize));

    if (infoHeader.biSize >= 12)
    {
        read(fil_r, infoHeader.biWidth, sizeof(infoHeader.biWidth));
        read(fil_r, infoHeader.biHeight, sizeof(infoHeader.biHeight));
        read(fil_r, infoHeader.biPlanes, sizeof(infoHeader.biPlanes));
        read(fil_r, infoHeader.biBitCount, sizeof(infoHeader.biBitCount));
    }

    int colorsCount = infoHeader.biBitCount >> 3;
    if (colorsCount < 3)
    {
        colorsCount = 3;
    }

    int bitsOnColor = infoHeader.biBitCount / colorsCount;
    int maskValue = (1 << bitsOnColor) - 1;

    if (infoHeader.biSize >= 40)
    {
        read(fil_r, infoHeader.biCompression, sizeof(infoHeader.biCompression));
        read(fil_r, infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage));
        read(fil_r, infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter));
        read(fil_r, infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter));
        read(fil_r, infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed));
        read(fil_r, infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant));
    }

    infoHeader.biRedMask = 0;
    infoHeader.biGreenMask = 0;
    infoHeader.biBlueMask = 0;

    if (infoHeader.biSize >= 52)
    {
        read(fil_r, infoHeader.biRedMask, sizeof(infoHeader.biRedMask));
        read(fil_r, infoHeader.biGreenMask, sizeof(infoHeader.biGreenMask));
        read(fil_r, infoHeader.biBlueMask, sizeof(infoHeader.biBlueMask));
    }


    if (infoHeader.biSize >= 56)
    {
        read(fil_r, infoHeader.biAlphaMask, sizeof(infoHeader.biAlphaMask));
    }
    else
    {
        infoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
    }

    if (infoHeader.biSize >= 108)
    {
        read(fil_r, infoHeader.biCSType, sizeof(infoHeader.biCSType));
        read(fil_r, infoHeader.biEndpoints, sizeof(infoHeader.biEndpoints));
        read(fil_r, infoHeader.biGammaRed, sizeof(infoHeader.biGammaRed));
        read(fil_r, infoHeader.biGammaGreen, sizeof(infoHeader.biGammaGreen));
        read(fil_r, infoHeader.biGammaBlue, sizeof(infoHeader.biGammaBlue));
    }

    if (infoHeader.biSize >= 124)
    {
        read(fil_r, infoHeader.biIntent, sizeof(infoHeader.biIntent));
        read(fil_r, infoHeader.biProfileData, sizeof(infoHeader.biProfileData));
        read(fil_r, infoHeader.biProfileSize, sizeof(infoHeader.biProfileSize));
        read(fil_r, infoHeader.biReserved, sizeof(infoHeader.biReserved));
    }

    int linePadding = (4 - (infoHeader.biWidth * 3) % 4) % 4;

    pixels = new RGBQUAD[infoHeader.biHeight * infoHeader.biWidth];

    for (int i = 0; i < infoHeader.biHeight; ++i)
    {
        for (int j = 0; j < infoHeader.biWidth; ++j)
        {
            fil_r.read(reinterpret_cast<char*>(&pixels[getIndex(j, i)].rgbBlue), sizeof(unsigned char));
            fil_r.read(reinterpret_cast<char*>(&pixels[getIndex(j, i)].rgbGreen), sizeof(unsigned char));
            fil_r.read(reinterpret_cast<char*>(&pixels[getIndex(j, i)].rgbRed), sizeof(unsigned char));
        }
        fil_r.ignore(linePadding);
    }
}


BITMAPINFOHEADER ImD::getInfoHeader()
{
    return infoHeader;
}
BITMAPFILEHEADER ImD::getFileHeader()
{
    return fileHeader;
}
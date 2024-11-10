

#include "ImD.h"

int writeBMP(const std::string& fileName, const ImD& imageData)
{
    std::ofstream file(fileName, std::ios::binary);
    if (!file)
    {
        std::cerr << "Ошибка при создании файла: " << fileName << std::endl;
        return -1;
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
            file.write(reinterpret_cast<const char*>(&imageData.pixels[i][j].rgbBlue), sizeof(unsigned char));
            file.write(reinterpret_cast<const char*>(&imageData.pixels[i][j].rgbGreen), sizeof(unsigned char));
            file.write(reinterpret_cast<const char*>(&imageData.pixels[i][j].rgbRed), sizeof(unsigned char));
        }

        file.write(reinterpret_cast<const char*>(padding), linePadding);
    }

    file.close();
    std::cout << "Файл успешно записан: " << fileName << std::endl;
    return 1;
}






void ImD::rotate90ContClockwise(std::string rFileName)
{

    ImD* rotatedImage = new ImD(*this);


    rotatedImage->infoHeader.biWidth = this->infoHeader.biHeight;
    rotatedImage->infoHeader.biHeight = this->infoHeader.biWidth;


    rotatedImage->pixels = new RGBQUAD*[rotatedImage->infoHeader.biHeight];
    for (unsigned int i = 0; i < rotatedImage->infoHeader.biHeight; ++i)
    {
        rotatedImage->pixels[i] = new RGBQUAD[rotatedImage->infoHeader.biWidth];
    }


    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            rotatedImage->pixels[j][this->infoHeader.biHeight - 1 - i] = this->pixels[i][j];
        }
    }

    writeBMP(rFileName,*rotatedImage);
    for (unsigned int i=0; i<rotatedImage->infoHeader.biHeight; ++i)
    {
        delete [] rotatedImage->pixels[i];
    }
    delete[] rotatedImage->pixels;

}



void ImD::rotate90Clockwise(std::string rFileName, bool WGaus)
{

    ImD* rotatedImage = new ImD(*this);


    rotatedImage->infoHeader.biWidth = this->infoHeader.biHeight;
    rotatedImage->infoHeader.biHeight = this->infoHeader.biWidth;


    rotatedImage->pixels = new RGBQUAD*[rotatedImage->infoHeader.biHeight];
    for (unsigned int i = 0; i < rotatedImage->infoHeader.biHeight; ++i)
    {
        rotatedImage->pixels[i] = new RGBQUAD[rotatedImage->infoHeader.biWidth];
    }


    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            rotatedImage->pixels[j][i] = this->pixels[i][this->infoHeader.biWidth - 1 -j];
        }
    }

    writeBMP(rFileName,*rotatedImage);

    if (WGaus)
    {
        for (unsigned int i=0; i<this->infoHeader.biHeight; ++i)
        {
            delete [] this->pixels[i];
        }
        delete[] this->pixels;

        applyGaussianFilter(rotatedImage);

        writeBMP(rFileName+"_withGaus.bmp",*rotatedImage);
    }

    for (unsigned int i=0; i<rotatedImage->infoHeader.biHeight; ++i)
    {
        delete [] rotatedImage->pixels[i];
    }
    delete[] rotatedImage->pixels;

}


void ImD::applyGaussianFilter(ImD* image)
{

    RGBQUAD** newPixels = new RGBQUAD*[image->infoHeader.biHeight];
    for (unsigned int i = 0; i < image->infoHeader.biHeight; ++i)
    {
        newPixels[i] = new RGBQUAD[image->infoHeader.biWidth];
    }


    for (unsigned int y = 1; y < image->infoHeader.biHeight - 1; ++y)
    {
        for (unsigned int x = 1; x < image->infoHeader.biWidth - 1; ++x)
        {
            double red = 0.0, green = 0.0, blue = 0.0;


            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int pixelY = y + ky;
                    int pixelX = x + kx;


                    RGBQUAD currentPixel = image->pixels[pixelY][pixelX];


                    red += currentPixel.rgbRed * gaussianKernel[ky + 1][kx + 1];
                    green += currentPixel.rgbGreen * gaussianKernel[ky + 1][kx + 1];
                    blue += currentPixel.rgbBlue * gaussianKernel[ky + 1][kx + 1];
                }
            }


            newPixels[y][x].rgbRed = static_cast<unsigned char>(red);
            newPixels[y][x].rgbGreen = static_cast<unsigned char>(green);
            newPixels[y][x].rgbBlue = static_cast<unsigned char>(blue);
        }
    }

    for (unsigned int i = 0; i < image->infoHeader.biHeight; ++i)
    {
        newPixels[0][i]=newPixels[1][i];
    }

    for (unsigned int i = 0; i < image->infoHeader.biHeight; ++i)
    {
        newPixels[image->infoHeader.biHeight-1][i]=newPixels[image->infoHeader.biHeight-2][i];
    }

    for (unsigned int i = 0; i < image->infoHeader.biHeight; ++i)
    {
        newPixels[i][0]=newPixels[i][1];
        newPixels[i][image->infoHeader.biWidth-1]=newPixels[i][image->infoHeader.biWidth-2];
        delete[] image->pixels[i];

        image->pixels[i] = newPixels[i];
    }
    delete[] newPixels;

}
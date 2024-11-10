/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/


#include "BMP_reader.h"
#include "BMP_writer.h"







int writeBMP(const std::string& fileName, const ImD& imageData)
{
    std::ofstream file(fileName, std::ios::binary);
    if (!file)
    {
        std::cerr << "Ошибка при создании файла: " << fileName << std::endl;
        return -1;
    }

    // Запись заголовка файла
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfType), sizeof(imageData.fileHeader.bfType));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfSize), sizeof(imageData.fileHeader.bfSize));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfReserved1), sizeof(imageData.fileHeader.bfReserved1));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfReserved2), sizeof(imageData.fileHeader.bfReserved2));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfOffBits), sizeof(imageData.fileHeader.bfOffBits));

    // Запись заголовка информации об изображении
    file.write(reinterpret_cast<const char*>(&imageData.infoHeader), (int)imageData.infoHeader.biSize);

    // Вычисление отступа (padding) для каждой строки
    int linePadding = (4 - (imageData.infoHeader.biWidth * 3) % 4) % 4;
    unsigned char padding[3] = {0, 0, 0}; // Буфер для записи отступа

    // Запись пикселей изображения
    for (int i = 0; i < imageData.infoHeader.biHeight; ++i)
    {
        for (int j = 0; j < imageData.infoHeader.biWidth; ++j)
        {
            file.write(reinterpret_cast<const char*>(&imageData.pixels[i][j].rgbBlue), sizeof(unsigned char));
            file.write(reinterpret_cast<const char*>(&imageData.pixels[i][j].rgbGreen), sizeof(unsigned char));
            file.write(reinterpret_cast<const char*>(&imageData.pixels[i][j].rgbRed), sizeof(unsigned char));
        }
        // Запись отступа в конце строки
        file.write(reinterpret_cast<const char*>(padding), linePadding);
    }

    file.close();
    std::cout << "Файл успешно записан: " << fileName << std::endl;
    return 1;
}






void ImD::rotate90ContClockwise(std::string rFileName)
{

    ImD* rotatedImage = new ImD(*this);

    // Обновляем ширину и высоту
    rotatedImage->infoHeader.biWidth = this->infoHeader.biHeight;
    rotatedImage->infoHeader.biHeight = this->infoHeader.biWidth;

    // Создаем новый массив для пикселей
    rotatedImage->pixels = new RGBQUAD*[rotatedImage->infoHeader.biHeight];
    for (unsigned int i = 0; i < rotatedImage->infoHeader.biHeight; ++i)
    {
        rotatedImage->pixels[i] = new RGBQUAD[rotatedImage->infoHeader.biWidth];
    }

    // Переносим пиксели с поворотом
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

    // Обновляем ширину и высоту
    rotatedImage->infoHeader.biWidth = this->infoHeader.biHeight;
    rotatedImage->infoHeader.biHeight = this->infoHeader.biWidth;

    // Создаем новый массив для пикселей
    rotatedImage->pixels = new RGBQUAD*[rotatedImage->infoHeader.biHeight];
    for (unsigned int i = 0; i < rotatedImage->infoHeader.biHeight; ++i)
    {
        rotatedImage->pixels[i] = new RGBQUAD[rotatedImage->infoHeader.biWidth];
    }

    // Переносим пиксели с поворотом
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
    // Создаем копию массива пикселей, чтобы хранить результаты
    RGBQUAD** newPixels = new RGBQUAD*[image->infoHeader.biHeight];
    for (unsigned int i = 0; i < image->infoHeader.biHeight; ++i)
    {
        newPixels[i] = new RGBQUAD[image->infoHeader.biWidth];
    }

    // Обходим каждый пиксель изображения (исключая границы)
    for (unsigned int y = 1; y < image->infoHeader.biHeight - 1; ++y)
    {
        for (unsigned int x = 1; x < image->infoHeader.biWidth - 1; ++x)
        {
            double red = 0.0, green = 0.0, blue = 0.0;

            // Применяем Гауссово ядро к окружающим пикселям
            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int pixelY = y + ky;
                    int pixelX = x + kx;

                    // Доступ к текущему пикселю
                    RGBQUAD currentPixel = image->pixels[pixelY][pixelX];

                    // Применяем ядро к каждому цветовому каналу
                    red += currentPixel.rgbRed * gaussianKernel[ky + 1][kx + 1];
                    green += currentPixel.rgbGreen * gaussianKernel[ky + 1][kx + 1];
                    blue += currentPixel.rgbBlue * gaussianKernel[ky + 1][kx + 1];
                }
            }

            // Записываем новые значения в массив newPixels
            newPixels[y][x].rgbRed = static_cast<unsigned char>(red);
            newPixels[y][x].rgbGreen = static_cast<unsigned char>(green);
            newPixels[y][x].rgbBlue = static_cast<unsigned char>(blue);
        }
    }

    /*
        for (unsigned int y = 0; y < image->infoHeader.biHeight; ++y) {
            for (unsigned int x = 0; x < image->infoHeader.biWidth; ++x) {
                if (y == 0 || y == image->infoHeader.biHeight - 1 || x == 0 || x == image->infoHeader.biWidth - 1) {
                    // Дублируем значение из ближайшего внутреннего пикселя
                    newPixels[y][x] = image->pixels[y][x];
                }
            }
        }
    */

    // Заменяем старые пиксели новыми
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
        /*
        if (i=0){
            delete[] image->pixels[i];
            image->pixels[i] = newPixels[i+1];
        }
        else if (i=image->infoHeader.biHeight-1){
            delete[] image->pixels[i];
            image->pixels[i] = newPixels[i-1];
        }
        else {

            newPixels[i][0]=newPixels[i][1];
            newPixels[i][image->infoHeader.biWidth-1]=newPixels[i][image->infoHeader.biWidth-2];

            delete[] image->pixels[i];
            image->pixels[i] = newPixels[i];
        }
        */
        newPixels[i][0]=newPixels[i][1];
        newPixels[i][image->infoHeader.biWidth-1]=newPixels[i][image->infoHeader.biWidth-2];




        delete[] image->pixels[i];

        image->pixels[i] = newPixels[i];
    }
    delete[] newPixels;

}













int reader(std::string fileName,bool need_clock,bool need_contrclock,bool need_Gaus)
{



// открываем файл
    std::ifstream fil_r(fileName, std::ifstream::binary);
// заголовoк изображения
    BITMAPFILEHEADER fileHeader;
    read(fil_r, fileHeader.bfType, sizeof(fileHeader.bfType));
    read(fil_r, fileHeader.bfSize, sizeof(fileHeader.bfSize));
    read(fil_r, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
    read(fil_r, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
    read(fil_r, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));

// информация изображения
    BITMAPINFOHEADER fileInfoHeader;
    read(fil_r, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));

// bmp core
    if (fileInfoHeader.biSize >= 12)
    {
        read(fil_r, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
        read(fil_r, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
        read(fil_r, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
        read(fil_r, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
    }

// получаем информацию о битности
    int colorsCount = fileInfoHeader.biBitCount >> 3;
    if (colorsCount < 3)
    {
        colorsCount = 3;
    }

    int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
    int maskValue = (1 << bitsOnColor) - 1;

// bmp v1
    if (fileInfoHeader.biSize >= 40)
    {
        read(fil_r, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
        read(fil_r, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
        read(fil_r, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));
        read(fil_r, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));
        read(fil_r, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
        read(fil_r, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
    }

// bmp v2
    fileInfoHeader.biRedMask = 0;
    fileInfoHeader.biGreenMask = 0;
    fileInfoHeader.biBlueMask = 0;

    if (fileInfoHeader.biSize >= 52)
    {
        read(fil_r, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
        read(fil_r, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
        read(fil_r, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
    }

/*
    if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0)
    {
        fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
        fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
        fileInfoHeader.biBlueMask = maskValue;
    }
*/


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

//
//
// вывод
    /*
     for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
        for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
            std::cout << std::hex
                << +rgbInfo[i][j].rgbRed << " "
                << +rgbInfo[i][j].rgbGreen << " "
                << +rgbInfo[i][j].rgbBlue << " "
                << +rgbInfo[i][j].rgbReserved
                << std::endl;
        }
     std::cout << std::endl;
     }
    */


//std::cout<<(int)rgbInfo[0][0].rgbRed<<(int)rgbInfo[0][0].rgbGreen<<(int)rgbInfo[0][0].rgbBlue<<(int)rgbInfo[0][0].rgbReserved<<std::endl;
//std::cout<<fileInfoHeader.biBitCount;
//writer(fileName,rgbInfo[fileInfoHeader.biHeight-1][fileInfoHeader.biWidth-1].rgbReserved);






    ImD* orig = new ImD(fileHeader,fileInfoHeader,rgbInfo,linePadding);


    for (unsigned int i=0; i<fileInfoHeader.biHeight; ++i)
    {
        delete [] rgbInfo[i];
    }
    delete[] rgbInfo;

//std::cout<<fileInfoHeader.biBitCount;
//std::cout<<fileInfoHeader.biBitCount;
//writeBMP(fileName+"_new.bmp",*orig);
    if (need_contrclock){
        orig->rotate90ContClockwise(fileName+"_rotContC.bmp");
    }
    if (need_clock){
        if (need_Gaus){
            orig->rotate90Clockwise(fileName+"_rotC.bmp",1);
        }
        else{
            orig->rotate90Clockwise(fileName+"_rotC.bmp",0);
        }
    }

    








    /*
    ImD *cartinka =new ImD(fileInfoHeader.biHeight,fileInfoHeader.biWidth,fileHeader.bfOffBits);
    cartinka->hdr={'4d','4d'};
    std::cout<<cartinka.hdr[0]<<cartinka.hdr[1]<<std::endl;
    */
    /*
     std::string **fil = new std::string*[fileInfoHeader.biHeight];
     fil[0]= new std::string[1];
     for (unsigned int i = 1; i < fileInfoHeader.biHeight+1; i++) {
         fil[i] = new RGBQUAD[fileInfoHeader.biWidth];
     }
     for (unsigned int i = 1; i < fileInfoHeader.biHeight+1; i++) {
        for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {

            fil[i+1]=rgbInfo[i-1][j];
     }
    */
    return 1;
}
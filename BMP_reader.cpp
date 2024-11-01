/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/


#include "BMP_reader.h"
#include "BMP_writer.h"
// bit extract
unsigned char bitextract(const unsigned int byte, const unsigned int mask) {
    if (mask == 0) {
        return 0;
    }
 
    // определение количества нулевых бит справа от маски
    int
        maskBufer = mask,
        maskPadding = 0;
 
    while (!(maskBufer & 1)) {
        maskBufer >>= 1;
        maskPadding++;
    }
 
    // применение маски и смещение
    return (byte & mask) >> maskPadding;
}
int reader(std::string fileName)
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
 if (fileInfoHeader.biSize >= 12) {
     read(fil_r, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
     read(fil_r, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
     read(fil_r, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
     read(fil_r, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
 }

 // получаем информацию о битности
 int colorsCount = fileInfoHeader.biBitCount >> 3;
 if (colorsCount < 3) {
     colorsCount = 3;
 }

 int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
 int maskValue = (1 << bitsOnColor) - 1;

 // bmp v1
 if (fileInfoHeader.biSize >= 40) {
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

 if (fileInfoHeader.biSize >= 52) {
     read(fil_r, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
     read(fil_r, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
     read(fil_r, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
 }

// если маска не задана, то ставим маску по умолчанию
 if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0) {
     fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
     fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
     fileInfoHeader.biBlueMask = maskValue;
 }

 // bmp v3
 if (fileInfoHeader.biSize >= 56) {
     read(fil_r, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
 } else {
     fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
 }

 // bmp v4
 if (fileInfoHeader.biSize >= 108) {
     read(fil_r, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
     read(fil_r, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
     read(fil_r, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
     read(fil_r, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
     read(fil_r, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
 }

 // bmp v5
 if (fileInfoHeader.biSize >= 124) {
     read(fil_r, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
     read(fil_r, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
     read(fil_r, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
     read(fil_r, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
 }

     // проверка на поддерку этой версии формата
 if (fileInfoHeader.biSize != 12 && fileInfoHeader.biSize != 40 && fileInfoHeader.biSize != 52 &&
     fileInfoHeader.biSize != 56 && fileInfoHeader.biSize != 108 && fileInfoHeader.biSize != 124) {
     std::cout << "Error: Unsupported BMP format." << std::endl;
     return 0;
 }

 if (fileInfoHeader.biBitCount != 16 && fileInfoHeader.biBitCount != 24 && fileInfoHeader.biBitCount != 32) {
     std::cout << "Error: Unsupported BMP bit count." << std::endl;
     return 0;
 }

 if (fileInfoHeader.biCompression != 0 && fileInfoHeader.biCompression != 3) {
     std::cout << "Error: Unsupported BMP compression." << std::endl;
     return 0;
 }

 // rgb info
 RGBQUAD **rgbInfo = new RGBQUAD*[fileInfoHeader.biHeight];

 for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
     rgbInfo[i] = new RGBQUAD[fileInfoHeader.biWidth];
 }

 // определение размера отступа в конце каждой строки
 int linePadding = ((fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) & 3;

 // чтение
 unsigned int bufer;
 char buf ;

 for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++) {
     for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++) {
         //read(fil_r, bufer, fileInfoHeader.biBitCount / 8);

        
         //memcpy(,buf,1);
         fil_r.get(buf);
         std::cout<<(int)(unsigned char)buf<<std::endl;
         rgbInfo[i][j].rgbBlue=(unsigned char)buf;

         fil_r.get(buf);
         rgbInfo[i][j].rgbGreen=(unsigned char)buf;
         std::cout<<(int)(unsigned char)buf<<std::endl;
         
         fil_r.get(buf);
         std::cout<<(int)(unsigned char)buf<<std::endl;
         rgbInfo[i][j].rgbRed=(unsigned char)buf;

         //fil_r.get(buf);
         //std::cout<<(int)(unsigned char)buf<<std::endl;
         //rgbInfo[i][j].rgbReserved=(unsigned char)buf;
         //rgbInfo[i][j].rgbGreen = bitextract(bufer, fileInfoHeader.biGreenMask);
         //rgbInfo[i][j].rgbBlue = bitextract(bufer, fileInfoHeader.biBlueMask);
         //rgbInfo[i][j].rgbReserved = bitextract(bufer, fileInfoHeader.biAlphaMask);
     }
     fil_r.seekg(linePadding, std::ios_base::cur);
 }

 // вывод
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


 std::cout<<(int)rgbInfo[0][0].rgbRed<<(int)rgbInfo[0][0].rgbGreen<<(int)rgbInfo[0][0].rgbBlue<<(int)rgbInfo[0][0].rgbReserved<<std::endl;
 std::cout<<fileInfoHeader.biBitCount;
 writer("examples_BMP/1_bmp_24.bmp",rgbInfo[fileInfoHeader.biHeight-1][fileInfoHeader.biWidth-1].rgbReserved);
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
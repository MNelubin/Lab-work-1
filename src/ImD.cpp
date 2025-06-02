/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/
#include "ImD.h"
#include <omp.h>

/**
 * @brief Template function to read data of a specific type from an input file stream.
 * @tparam Type The data type to read.
 * @param fp The input file stream.
 * @param result Reference to store the read data.
 * @param size The number of bytes to read.
 */
template <typename Type>
void ImD::read(std::ifstream &fp, Type &result, std::size_t size)
{
    fp.read(reinterpret_cast<char*>(&result), size);
}

/**
 * @brief Copy constructor.
 * @param image The ImD object to copy.
 */
ImD::ImD(const ImD& image) : fileHeader(image.fileHeader), infoHeader(image.infoHeader), padding(image.padding)
{
    const unsigned int total_pixels = image.infoHeader.biHeight * image.infoHeader.biWidth;

    if (total_pixels == 0)
    {
        this->pixels = nullptr;
    }
    else
    {
        this->pixels = new RGBQUAD[total_pixels];
        if (image.pixels != nullptr)
        {
            std::copy(image.pixels, image.pixels + total_pixels, this->pixels);
        }
        else
        {

        }
    }
}

/**
 * @brief Default constructor.
 * Initializes an empty ImD object.
 */
ImD::ImD()
    : fileHeader(), infoHeader(), pixels(nullptr), padding(0)
{
}

/**
 * @brief Constructor with parameters.
 * @param fh The BITMAPFILEHEADER for the image.
 * @param ih The BITMAPINFOHEADER for the image.
 * @param pixelData Pointer to the RGBQUAD pixel data.
 * @param pad The padding value for the image rows.
 */
ImD::ImD(const BITMAPFILEHEADER& fh, const BITMAPINFOHEADER& ih, RGBQUAD* pixelData, int pad)
    : fileHeader(fh), infoHeader(ih), padding(pad)
{
    const unsigned int total_pixels = static_cast<size_t>(infoHeader.biHeight) * static_cast<size_t>(infoHeader.biWidth);
    pixels = new RGBQUAD[total_pixels];
    if (pixelData != nullptr)
    {
        std::copy(pixelData, pixelData + total_pixels, pixels);
    }
    else
    {
    }
}

/**
 * @brief Destructor.
 * Frees the allocated memory for pixel data.
 */
ImD::~ImD()
{
    delete[] pixels;
}

/**
 * @brief Copy assignment operator.
 * @param other The ImD object to assign from.
 * @return Reference to this ImD object.
 */
ImD& ImD::operator=(const ImD& other)
{
    if (this == &other)
    {
        return *this;
    }

    if (pixels)
    {
        delete[] pixels;
        pixels = nullptr;
    }

    fileHeader = other.fileHeader;
    infoHeader = other.infoHeader;
    padding = other.padding;

    const unsigned int total_pixels = infoHeader.biHeight * infoHeader.biWidth;
    if (total_pixels == 0)
    {
        pixels = nullptr;
    }
    else
    {
        pixels = new RGBQUAD[total_pixels];
        if (other.pixels != nullptr)
        {
            for (unsigned int i = 0; i < infoHeader.biHeight; ++i)
            {
                for (unsigned int j = 0; j < infoHeader.biWidth; ++j)
                {
                    pixels[getIndex(j, i)] = other.pixels[other.getIndex(j, i)];
                }
            }
        }
        else
        {

        }
    }

    return *this;
}

/**
 * @brief Writes BMP image data to a file.
 * @param fileName The name of the file to write the BMP image to.
 * @param imageData The ImD object containing the image data to write.
 */
void ImD::writeBMP(const std::string& fileName, const ImD& imageData)
{
    std::ofstream file(fileName, std::ios::binary);
    if (!file)
    {
        return;
    }

    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfType), sizeof(imageData.fileHeader.bfType));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfSize), sizeof(imageData.fileHeader.bfSize));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfReserved1), sizeof(imageData.fileHeader.bfReserved1));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfReserved2), sizeof(imageData.fileHeader.bfReserved2));
    file.write(reinterpret_cast<const char*>(&imageData.fileHeader.bfOffBits), sizeof(imageData.fileHeader.bfOffBits));

    file.write(reinterpret_cast<const char*>(&imageData.infoHeader), (int)imageData.infoHeader.biSize);

    if (imageData.pixels == nullptr && (imageData.infoHeader.biHeight > 0 && imageData.infoHeader.biWidth > 0))
    {
        file.close();
        return;
    }

    if (imageData.pixels != nullptr || (imageData.infoHeader.biHeight == 0 || imageData.infoHeader.biWidth == 0))
    {
        int linePadding = (4 - (imageData.infoHeader.biWidth * 3) % 4) % 4;
        unsigned char padding_bytes[3] = {0, 0, 0};

        for (unsigned int i = 0; i < imageData.infoHeader.biHeight; ++i)
        {
            for (unsigned int j = 0; j < imageData.infoHeader.biWidth; ++j)
            {
                file.write(reinterpret_cast<const char*>(&imageData.pixels[imageData.getIndex(j, i)].rgbBlue), sizeof(unsigned char));
                file.write(reinterpret_cast<const char*>(&imageData.pixels[imageData.getIndex(j, i)].rgbGreen), sizeof(unsigned char));
                file.write(reinterpret_cast<const char*>(&imageData.pixels[imageData.getIndex(j, i)].rgbRed), sizeof(unsigned char));
            }
            file.write(reinterpret_cast<const char*>(padding_bytes), linePadding);
        }
    }

    file.close();
}

/**
 * @brief Sets the height of the image.
 * @param h The new height.
 */
void ImD::setHeight(unsigned int h)
{
    this->infoHeader.biHeight = h;
}

/**
 * @brief Sets the width of the image.
 * @param w The new width.
 */
void ImD::setWidth(unsigned int w)
{
    this->infoHeader.biWidth = w;
}

/**
 * @brief Sets a single pixel's data at a given 1D coordinate.
 * @param cord The 1D index of the pixel.
 * @param Data The RGBQUAD data for the pixel.
 */
void ImD::setPixel_1(unsigned int cord, RGBQUAD Data)
{
    pixels[cord] = Data;
}

/**
 * @brief Gets the width of the image.
 * @return The width of the image in pixels.
 */
unsigned int ImD::getWidth()
{
    return infoHeader.biWidth;
}

/**
 * @brief Gets the height of the image.
 * @return The height of the image in pixels.
 */
unsigned int ImD::getHeight()
{
    return infoHeader.biHeight;
}

/**
 * @brief Calculates the 1D index from 2D coordinates.
 * @param x The x-coordinate (column).
 * @param y The y-coordinate (row).
 * @return The 1D index in the pixel array.
 */
unsigned int ImD::getIndex(unsigned int x, unsigned int y) const
{
    return y * infoHeader.biWidth + x;
}


/**
 * @brief Rotates the image 90 degrees counter-clockwise (sequential).
 * @param rFileName The name of the file to save the rotated image to.
 */
void ImD::rotate90ContClockwise(const std::string rFileName)
{
    if (this->pixels == nullptr && this->infoHeader.biHeight > 0 && this->infoHeader.biWidth > 0)
    {
        return;
    }

    const unsigned int new_width = infoHeader.biHeight;
    const unsigned int new_height = infoHeader.biWidth;
    const unsigned int total_new_pixels = new_width * new_height;

    ImD* rotatedImage = new ImD(*this);
    if (rotatedImage->pixels == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    rotatedImage->setWidth(new_width);
    rotatedImage->setHeight(new_height);

    RGBQUAD* new_pixel_array = nullptr;
    if (total_new_pixels > 0)
    {
        new_pixel_array = new RGBQUAD[total_new_pixels];
    }

    if (new_pixel_array == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            unsigned int srcindx = getIndex(j, i);
            unsigned int dest_x = i;
            unsigned int dest_y = new_height - 1 - j;
            unsigned int dest_idx = dest_y * new_width + dest_x;
            if (new_pixel_array && this->pixels)
            {
                new_pixel_array[dest_idx] = this->pixels[srcindx];
            }
        }
    }
    delete[] rotatedImage->pixels;
    rotatedImage->pixels = new_pixel_array;

    rotatedImage->writeBMP(rFileName+"_ContClockwise.bmp",*rotatedImage);

    delete rotatedImage;
}


// Sequential 90 deg clockwise rotation
/**
 * @brief Rotates the image 90 degrees clockwise (sequential).
 * @param rFileName The name of the file to save the rotated image to.
 * @param WGaus If true, applies Gaussian filter after rotation (default is false).
 */
void ImD::rotate90Clockwise(const std::string rFileName, bool WGaus)
{
    if (this->pixels == nullptr && this->infoHeader.biHeight > 0 && this->infoHeader.biWidth > 0)
    {
        return;
    }

    const unsigned int new_width = infoHeader.biHeight;
    const unsigned int new_height = infoHeader.biWidth;
    const unsigned int total_new_pixels = new_width * new_height;

    ImD* rotatedImage= new ImD(*this);
    if (rotatedImage->pixels == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    rotatedImage->setWidth(new_width);
    rotatedImage->setHeight(new_height);

    RGBQUAD* new_pixel_array = nullptr;
    if (total_new_pixels > 0)
    {
        new_pixel_array = new RGBQUAD[total_new_pixels];
    }

    if (new_pixel_array == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            unsigned int srcindx = getIndex(j,i);
            unsigned int dest_x = new_width - 1 - i;
            unsigned int dest_y = j;
            unsigned int dest_idx = dest_y * new_width + dest_x;
            if (new_pixel_array && this->pixels)
            {
                new_pixel_array[dest_idx] = this->pixels[srcindx];
            }
        }
    }
    delete[] rotatedImage->pixels;
    rotatedImage->pixels = new_pixel_array;

    rotatedImage->writeBMP(rFileName+"_Clockwise.bmp",*rotatedImage);

    delete rotatedImage;
}

// Sets the pixel data pointer to null.
/**
 * @brief Sets the internal pixel pointer to nullptr.
 * This is typically used when transferring ownership or clearing data.
 */
void ImD::setPixels_null()
{
    pixels=nullptr;
}

// Gets the padding of the image.
/**
 * @brief Gets the padding bytes for each row.
 * @return The padding value.
 */
int ImD::getPadding()
{
    return padding;
}

// Sets the entire pixel array.
/**
 * @brief Sets all pixel data from a given array.
 * @param Data Pointer to the new RGBQUAD pixel data array.
 */
void ImD::setPixel_all(RGBQUAD* Data)
{
    pixels=Data;
}

// Gets the pointer to the entire pixel array.
/**
 * @brief Gets a pointer to all pixel data.
 * @return Pointer to the RGBQUAD pixel data array.
 */
RGBQUAD* ImD::getPixels_all()
{
    return pixels;
}


// Sequential Gaussian filter
/**
 * @brief Applies a Gaussian filter to the image (sequential).
 * @param fileName The name of the file to save the filtered image to.
 */
void ImD::applyGaussianFilter(const std::string& fileName)
{
    const unsigned int total_pixels = infoHeader.biHeight * infoHeader.biWidth;
    if (this->pixels == nullptr && total_pixels > 0)
    {
        return;
    }
    RGBQUAD* newPixels = nullptr;
    if (total_pixels > 0)
    {
        newPixels = new RGBQUAD[total_pixels];
    }

    if (newPixels == nullptr && total_pixels > 0)
    {
        return;
    }

    if (infoHeader.biWidth < 3 || infoHeader.biHeight < 3)
    {
        if (this->pixels && newPixels)
        {
            for (unsigned int i = 0; i < total_pixels; ++i)
            {
                newPixels[i] = this->pixels[i];
            }
        }
    }
    else
    {
        for (unsigned int y = 1; y < infoHeader.biHeight - 1; ++y)
        {
            for (unsigned int x = 1; x < infoHeader.biWidth - 1; ++x)
            {
                double red = 0.0, green = 0.0, blue = 0.0;

                for (int ky = -1; ky <= 1; ++ky)
                {
                    for (int kx = -1; kx <= 1; ++kx)
                    {
                        unsigned int pixelY = y + ky;
                        unsigned int pixelX = x + kx;

                        RGBQUAD currentPixel = pixels[getIndex(pixelX, pixelY)];

                        red += currentPixel.rgbRed * gaussianKernel[ky + 1][kx + 1];
                        green += currentPixel.rgbGreen * gaussianKernel[ky + 1][kx + 1];
                        blue += currentPixel.rgbBlue * gaussianKernel[ky + 1][kx + 1];
                    }
                }
                newPixels[getIndex(x, y)] = {static_cast<unsigned char>(blue), static_cast<unsigned char>(green), static_cast<unsigned char>(red)};
            }
        }

        for (unsigned int i = 0; i < infoHeader.biHeight; ++i)
        {
            if (infoHeader.biWidth > 1)
            {
                newPixels[getIndex(0, i)] = newPixels[getIndex(1, i)];
                newPixels[getIndex(infoHeader.biWidth - 1, i)] = newPixels[getIndex(infoHeader.biWidth - 2, i)];
            }
            else if (infoHeader.biWidth == 1 && newPixels && this->pixels)
            {
                newPixels[getIndex(0,i)] = this->pixels[getIndex(0,i)];
            }
        }

        for (unsigned int i = 0; i < infoHeader.biWidth; ++i)
        {
            if (infoHeader.biHeight > 1)
            {
                newPixels[getIndex(i, 0)] = newPixels[getIndex(i, 1)];
                newPixels[getIndex(i, infoHeader.biHeight - 1)] = newPixels[getIndex(i, infoHeader.biHeight - 2)];
            }
            else if (infoHeader.biHeight == 1 && newPixels && this->pixels)
            {
                newPixels[getIndex(i,0)] = this->pixels[getIndex(i,0)];
            }
        }
    }

    delete[] pixels;
    pixels = newPixels;
    writeBMP(fileName+"_WGaus.bmp",*this);
}

// Reads BMP image data from a file.
/**
 * @brief Reads a BMP image from a file.
 * @param fileName The name of the BMP file to read.
 * @param need_clock Flag indicating if clockwise rotation is needed (currently unused in this method's direct logic but might be for subsequent calls).
 * @param need_contrclock Flag indicating if counter-clockwise rotation is needed (currently unused).
 * @param need_Gaus Flag indicating if Gaussian filter is needed (currently unused).
 */
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
        return;
    }

    read(fil_r, fileHeader.bfType, sizeof(fileHeader.bfType));
    read(fil_r, fileHeader.bfSize, sizeof(fileHeader.bfSize));
    read(fil_r, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
    read(fil_r, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
    read(fil_r, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));

    if (fileHeader.bfType != 0x4D42)
    {
        fil_r.close();
        return;
    }

    read(fil_r, infoHeader.biSize, sizeof(infoHeader.biSize));

    if (infoHeader.biSize < 40)
    {
        fil_r.close();
        return;
    }

    if (infoHeader.biSize >= 12)
    {
        uint32_t tempWidth, tempHeight;
        read(fil_r, tempWidth, sizeof(uint32_t));
        read(fil_r, tempHeight, sizeof(uint32_t));
        infoHeader.biWidth = tempWidth;
        infoHeader.biHeight = tempHeight;


        read(fil_r, infoHeader.biPlanes, sizeof(infoHeader.biPlanes));
        read(fil_r, infoHeader.biBitCount, sizeof(infoHeader.biBitCount));

        if (infoHeader.biPlanes != 1)
        {
            fil_r.close();
            return;
        }

        if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32)
        {
            fil_r.close();
            return;
        }
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

    fil_r.seekg(fileHeader.bfOffBits, std::ios::beg);
    if (fil_r.fail())
    {
        fil_r.close();
        return;
    }

    int linePadding = (4 - (infoHeader.biWidth * 3) % 4) % 4;

    const unsigned int total_pixels = infoHeader.biHeight * infoHeader.biWidth;
    pixels = new RGBQUAD[total_pixels];

    if (pixels == nullptr && total_pixels > 0)
    {
        fil_r.close();
        return;
    }

    for (unsigned int i = 0; i < infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < infoHeader.biWidth; ++j)
        {
            fil_r.read(reinterpret_cast<char*>(&pixels[getIndex(j, i)].rgbBlue), sizeof(unsigned char));
            fil_r.read(reinterpret_cast<char*>(&pixels[getIndex(j, i)].rgbGreen), sizeof(unsigned char));
            fil_r.read(reinterpret_cast<char*>(&pixels[getIndex(j, i)].rgbRed), sizeof(unsigned char));
        }
        fil_r.ignore(linePadding);
    }

    long long expected_pixel_data_size = (long long)infoHeader.biHeight * (infoHeader.biWidth * 3 + linePadding);
    long long actual_pixel_data_read = (long long)fil_r.tellg() - fileHeader.bfOffBits;

    if (actual_pixel_data_read < expected_pixel_data_size)
    {

    }
    else if (actual_pixel_data_read > expected_pixel_data_size)
    {

    }

    fil_r.close();

    long long calculated_file_size = fileHeader.bfOffBits + expected_pixel_data_size;
    if (fileHeader.bfSize != calculated_file_size)
    {

    }

    if (need_clock)
    {
        rotate90Clockwise(fileName, need_Gaus);
    }
    if (need_contrclock)
    {
        rotate90ContClockwise(fileName);
    }
    if (need_Gaus && !need_clock)
    {
        applyGaussianFilter(fileName);
    }
}

// Gets the BITMAPINFOHEADER of the image.
/**
 * @brief Gets the BMP info header.
 * @return The BITMAPINFOHEADER structure.
 */
BITMAPINFOHEADER ImD::getInfoHeader()
{
    return infoHeader;
}

// Parallel Gaussian filter
/**
 * @brief Applies a Gaussian filter to the image (parallel using OpenMP).
 * @param fileName The name of the file to save the filtered image to.
 */
void ImD::applyGaussianFilter_parallel(const std::string& fileName)
{
    const unsigned int total_pixels = infoHeader.biHeight * infoHeader.biWidth;
    if (this->pixels == nullptr && total_pixels > 0)
    {
        return;
    }
    RGBQUAD* newPixels = new RGBQUAD[total_pixels];


    #pragma omp parallel for collapse(2) schedule(static)
    for (unsigned int y = 1; y < infoHeader.biHeight - 1; ++y)
    {
        for (unsigned int x = 1; x < infoHeader.biWidth - 1; ++x)
        {
            double red = 0.0, green = 0.0, blue = 0.0;
            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    unsigned int pixelY = y + ky;
                    unsigned int pixelX = x + kx;
                    RGBQUAD currentPixel = pixels[getIndex(pixelX, pixelY)];
                    red += currentPixel.rgbRed * gaussianKernel[ky + 1][kx + 1];
                    green += currentPixel.rgbGreen * gaussianKernel[ky + 1][kx + 1];
                    blue += currentPixel.rgbBlue * gaussianKernel[ky + 1][kx + 1];
                }
            }
            newPixels[getIndex(x, y)] = {static_cast<unsigned char>(blue), static_cast<unsigned char>(green), static_cast<unsigned char>(red)};
        }
    }

    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < infoHeader.biHeight; ++i)
    {
        if (infoHeader.biWidth > 1)
        {
            newPixels[getIndex(0, i)] = newPixels[getIndex(1, i)];
            newPixels[getIndex(infoHeader.biWidth - 1, i)] = newPixels[getIndex(infoHeader.biWidth - 2, i)];
        }
        else if (infoHeader.biWidth == 1 && newPixels && this->pixels)
        {
            newPixels[getIndex(0,i)] = this->pixels[getIndex(0,i)];
        }
    }

    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < infoHeader.biWidth; ++i)
    {
        if (infoHeader.biHeight > 1)
        {
            newPixels[getIndex(i, 0)] = newPixels[getIndex(i, 1)];
            newPixels[getIndex(i, infoHeader.biHeight - 1)] = newPixels[getIndex(i, infoHeader.biHeight - 2)];
        }
        else if (infoHeader.biHeight == 1 && newPixels && this->pixels)
        {
            newPixels[getIndex(i,0)] = this->pixels[getIndex(i,0)];
        }
    }
    delete[] pixels;
    pixels = newPixels;
    writeBMP(fileName+"_WGaus_parallel.bmp",*this);
}

// Parallel 90 deg counterclockwise rotation
/**
 * @brief Rotates the image 90 degrees counter-clockwise (parallel using OpenMP).
 * @param rFileName The name of the file to save the rotated image to.
 */
void ImD::rotate90ContClockwise_parallel(const std::string rFileName)
{
    if (this->pixels == nullptr && this->infoHeader.biHeight > 0 && this->infoHeader.biWidth > 0)
    {
        return;
    }

    const unsigned int new_width = infoHeader.biHeight;
    const unsigned int new_height = infoHeader.biWidth;
    const unsigned int total_new_pixels = new_width * new_height;

    ImD* rotatedImage= new ImD(*this);
    if (rotatedImage->pixels == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    rotatedImage->setWidth(new_width);
    rotatedImage->setHeight(new_height);

    RGBQUAD* new_pixel_array = nullptr;
    if (total_new_pixels > 0)
    {
        new_pixel_array = new RGBQUAD[total_new_pixels];
    }

    if (new_pixel_array == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    #pragma omp parallel for collapse(2) schedule(static)
    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            unsigned int srcindx = getIndex(j, i);
            unsigned int dest_x = i;
            unsigned int dest_y = new_height - 1 - j;
            unsigned int dest_idx = dest_y * new_width + dest_x;
            if (new_pixel_array && this->pixels)
            {
                new_pixel_array[dest_idx] = this->pixels[srcindx];
            }
        }
    }
    delete[] rotatedImage->pixels;
    rotatedImage->pixels = new_pixel_array;

    rotatedImage->writeBMP(rFileName+"_ContClockwise_parallel.bmp",*rotatedImage);
    delete rotatedImage;
}

// Parallel 90 deg clockwise rotation
/**
 * @brief Rotates the image 90 degrees clockwise (parallel using OpenMP).
 * @param rFileName The name of the file to save the rotated image to.
 * @param WGaus If true, applies Gaussian filter after rotation (default is false).
 */
void ImD::rotate90Clockwise_parallel(const std::string rFileName, bool WGaus)
{
    if (this->pixels == nullptr && this->infoHeader.biHeight > 0 && this->infoHeader.biWidth > 0)
    {
        return;
    }

    const unsigned int new_width = infoHeader.biHeight;
    const unsigned int new_height = infoHeader.biWidth;
    const unsigned int total_new_pixels = new_width * new_height;

    ImD* rotatedImage= new ImD(*this);
    if (rotatedImage->pixels == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    rotatedImage->setWidth(new_width);
    rotatedImage->setHeight(new_height);

    RGBQUAD* new_pixel_array = nullptr;
    if (total_new_pixels > 0)
    {
        new_pixel_array = new RGBQUAD[total_new_pixels];
    }

    if (new_pixel_array == nullptr && total_new_pixels > 0)
    {
        delete rotatedImage;
        return;
    }

    #pragma omp parallel for collapse(2) schedule(static)
    for (unsigned int i = 0; i < this->infoHeader.biHeight; ++i)
    {
        for (unsigned int j = 0; j < this->infoHeader.biWidth; ++j)
        {
            unsigned int srcindx = getIndex(j,i);
            unsigned int dest_x = new_width - 1 - i;
            unsigned int dest_y = j;
            unsigned int dest_idx = dest_y * new_width + dest_x;
            if (new_pixel_array && this->pixels)
            {
                new_pixel_array[dest_idx] = this->pixels[srcindx];
            }
        }
    }
    delete[] rotatedImage->pixels;
    rotatedImage->pixels = new_pixel_array;

    rotatedImage->writeBMP(rFileName+"_Clockwise_parallel.bmp",*rotatedImage);

    delete rotatedImage;
}

// Gets the BITMAPFILEHEADER of the image.
/**
 * @brief Gets the BMP file header.
 * @return The BITMAPFILEHEADER structure.
 */
BITMAPFILEHEADER ImD::getFileHeader()
{
    return fileHeader;
}

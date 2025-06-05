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

/**
 * @brief Structure for the BMP file header.
 * Contains information about the BMP file, such as its type, size, and offset to pixel data.
 */
struct BITMAPFILEHEADER
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

/**
 * @brief Structure for the BMP information header.
 * Contains detailed information about the image, such as its dimensions, color depth, and compression type.
 */
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

/**
 * @brief Structure to represent a pixel in RGB format.
 * Stores the blue, green, and red color components of a pixel.
 */
struct RGBQUAD
{
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    // unsigned char rgbReserved;
};

/**
 * @brief Class for handling BMP image data and operations.
 * Provides functionalities to read, write, rotate, and apply filters to BMP images.
 */
class ImD
{
private:
    BITMAPFILEHEADER fileHeader; ///< BMP file header.
    BITMAPINFOHEADER infoHeader; ///< BMP info header.
    RGBQUAD* pixels;             ///< Pointer to the pixel data array.
    int padding;                 ///< Padding bytes for each row in the BMP file.
    const double gaussianKernel[3][3] = ///< 3x3 Gaussian kernel for filtering.
    {
        {1.0 / 16, 2.0 / 16, 1.0 / 16},
        {2.0 / 16, 4.0 / 16, 2.0 / 16},
        {1.0 / 16, 2.0 / 16, 1.0 / 16}
    };

public:
    /**
     * @brief Default constructor.
     * Initializes an empty ImD object.
     */
    ImD();

    /**
     * @brief Copy constructor.
     * @param image The ImD object to copy.
     */
    ImD(const ImD& image);

    /**
     * @brief Constructor with parameters.
     * @param fh The BITMAPFILEHEADER for the image.
     * @param ih The BITMAPINFOHEADER for the image.
     * @param pixelData Pointer to the RGBQUAD pixel data.
     * @param pad The padding value for the image rows.
     */
    ImD(const BITMAPFILEHEADER& fh, const BITMAPINFOHEADER& ih, RGBQUAD* pixelData, int pad);

    /**
     * @brief Destructor.
     * Frees the allocated memory for pixel data.
     */
    ~ImD();

    /**
     * @brief Reads a BMP image from a file.
     * @param fileName The name of the BMP file to read.
     * @param need_clock Flag indicating if clockwise rotation is needed (currently unused in this method's direct logic but might be for subsequent calls).
     * @param need_contrclock Flag indicating if counter-clockwise rotation is needed (currently unused).
     * @param need_Gaus Flag indicating if Gaussian filter is needed (currently unused).
     */
    void reader(std::string fileName, bool need_clock, bool need_contrclock, bool need_Gaus);

    /**
     * @brief Writes BMP image data to a file.
     * @param fileName The name of the file to write the BMP image to.
     * @param imageData The ImD object containing the image data to write.
     */
    void writeBMP(const std::string& fileName, const ImD& imageData);

    /**
     * @brief Sets the height of the image.
     * @param h The new height.
     */
    void setHeight(unsigned int h);

    /**
     * @brief Sets the width of the image.
     * @param w The new width.
     */
    void setWidth(unsigned int w);

    /**
     * @brief Sets a single pixel's data at a given 1D coordinate.
     * @param cord The 1D index of the pixel.
     * @param Data The RGBQUAD data for the pixel.
     */
    void setPixel_1(unsigned int cord, RGBQUAD Data);

    /**
     * @brief Sets all pixel data from a given array.
     * @param Data Pointer to the new RGBQUAD pixel data array.
     */
    void setPixel_all(RGBQUAD* Data);

    /**
     * @brief Sets the internal pixel pointer to nullptr.
     * This is typically used when transferring ownership or clearing data.
     */
    void setPixels_null();

    /**
     * @brief Gets the width of the image.
     * @return The width of the image in pixels.
     */
    unsigned int getWidth() const;

    /**
     * @brief Gets the height of the image.
     * @return The height of the image in pixels.
     */
    unsigned int getHeight() const;

    /**
     * @brief Calculates the 1D index from 2D coordinates.
     * @param x The x-coordinate (column).
     * @param y The y-coordinate (row).
     * @return The 1D index in the pixel array.
     */
    unsigned int getIndex(unsigned int x, unsigned int y) const;

    /**
     * @brief Gets the padding bytes for each row.
     * @return The padding value.
     */
    int getPadding();

    /**
     * @brief Gets a pointer to all pixel data.
     * @return Pointer to the RGBQUAD pixel data array.
     */
    RGBQUAD* getPixels_all() const;

    /**
     * @brief Gets the BMP info header.
     * @return The BITMAPINFOHEADER structure.
     */
    BITMAPINFOHEADER getInfoHeader() const;

    /**
     * @brief Gets the BMP file header.
     * @return The BITMAPFILEHEADER structure.
     */
    BITMAPFILEHEADER getFileHeader();

    /**
     * @brief Rotates the image 90 degrees clockwise (sequential).
     * @param rFileName The name of the file to save the rotated image to.
     * @param WGaus If true, applies Gaussian filter after rotation (default is false).
     */
    void rotate90Clockwise(const std::string rFileName, bool WGaus = false);

    /**
     * @brief Rotates the image 90 degrees counter-clockwise (sequential).
     * @param rFileName The name of the file to save the rotated image to.
     */
    void rotate90ContClockwise(const std::string rFileName);

    /**
     * @brief Applies a Gaussian filter to the image (sequential).
     * @param fileName The name of the file to save the filtered image to.
     */
    void applyGaussianFilter(const std::string& fileName);

    /**
     * @brief Rotates the image 90 degrees clockwise (parallel using OpenMP).
     * @param rFileName The name of the file to save the rotated image to.
     * @param WGaus If true, applies Gaussian filter after rotation (default is false).
     */
    void rotate90Clockwise_parallel(const std::string rFileName, bool WGaus = false);

    /**
     * @brief Rotates the image 90 degrees counter-clockwise (parallel using OpenMP).
     * @param rFileName The name of the file to save the rotated image to.
     */
    void rotate90ContClockwise_parallel(const std::string rFileName);

    /**
     * @brief Applies a Gaussian filter to the image (parallel using OpenMP).
     * @param fileName The name of the file to save the filtered image to.
     */
    void applyGaussianFilter_parallel(const std::string& fileName);

    /**
     * @brief Template function to read data of a specific type from an input file stream.
     * @tparam Type The data type to read.
     * @param fp The input file stream.
     * @param result Reference to store the read data.
     * @param size The number of bytes to read.
     */
    template <typename Type>
    void read(std::ifstream &fp, Type &result, std::size_t size);

    /**
     * @brief Copy assignment operator.
     * @param other The ImD object to assign from.
     * @return Reference to this ImD object.
     */
    ImD& operator=(const ImD& other);
#endif
};
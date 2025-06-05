#include "gtest/gtest.h"
#include "ImD.h" // Ensure the path to ImD.h is correct
#include <vector>
#include <fstream> // For read/write tests
#include <cstdio>  // For std::remove in read/write tests

/**
 * @brief Helper function to create a simple ImD object for tests.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param pixel_values Optional vector of RGBQUAD pixel values. If the size does not match width*height, a black image is created.
 * @return An ImD object.
 */
ImD createTestImD(unsigned int width, unsigned int height, const std::vector<RGBQUAD>& pixel_values = {}) {
    BITMAPFILEHEADER fh = {};
    BITMAPINFOHEADER ih = {};

    ih.biWidth = width;
    ih.biHeight = height;
    ih.biBitCount = 24; // Most tests will assume 24-bit
    ih.biSize = sizeof(BITMAPINFOHEADER);
    ih.biCompression = 0;      // BI_RGB
    ih.biPlanes = 1;
    ih.biSizeImage = width * height * 3; // Approximate size, can change with alignment

    fh.bfType = 0x4D42; // 'BM'
    fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    int row_stride = (width * 3 + 3) & ~3;
    ih.biSizeImage = row_stride * height;
    fh.bfSize = fh.bfOffBits + ih.biSizeImage;


    RGBQUAD* pixels_arr = nullptr;
    if (width > 0 && height > 0) {
        pixels_arr = new RGBQUAD[static_cast<size_t>(width) * height];
        if (pixel_values.size() == static_cast<size_t>(width) * height) {
            std::copy(pixel_values.begin(), pixel_values.end(), pixels_arr);
        } else {
            // Fill with black if data is not provided or size does not match
            for (size_t i = 0; i < static_cast<size_t>(width) * height; ++i) {
                pixels_arr[i] = {0, 0, 0}; // B, G, R
            }
        }
    }

    // ImD constructor creates a deep copy of pixelData
    ImD img(fh, ih, pixels_arr, (4 - (width * 3) % 4) % 4);

    delete[] pixels_arr; // pixels_arr was copied into img

    return img; // ImD copy or move constructor is used
}

/**
 * @brief Helper function to compare two ImD objects.
 * @param img1 The first ImD object.
 * @param img2 The second ImD object.
 * @return True if images are identical (headers and pixels), false otherwise.
 */
bool areImagesEqual(const ImD& img1, const ImD& img2) {
    if (img1.getWidth() != img2.getWidth() || img1.getHeight() != img2.getHeight()) {

        return false;
    }

    // Compare some key header fields
    // (more fields can be added if necessary)
    BITMAPINFOHEADER ih1 = img1.getInfoHeader();
    BITMAPINFOHEADER ih2 = img2.getInfoHeader();
    if (ih1.biWidth != ih2.biWidth || ih1.biHeight != ih2.biHeight ||
        ih1.biBitCount != ih2.biBitCount /*|| ih1.biCompression != ih2.biCompression*/) {
        // std::cout << "BITMAPINFOHEADER headers do not match." << std::endl;
        return false;
    }
    // Comparison of BITMAPFILEHEADER can also be added

    if (img1.getPixels_all() == nullptr && img2.getPixels_all() == nullptr) {
        return true; // Both have no pixels
    }
    if (img1.getPixels_all() == nullptr || img2.getPixels_all() == nullptr) {
        // std::cout << "One of the pixel pointers is nullptr, the other is not." << std::endl;
        return false; // One has pixels, the other does not
    }

    // Pixel-by-pixel comparison
    for (unsigned int y = 0; y < img1.getHeight(); ++y) {
        for (unsigned int x = 0; x < img1.getWidth(); ++x) {
            RGBQUAD p1 = img1.getPixels_all()[img1.getIndex(x, y)];
            RGBQUAD p2 = img2.getPixels_all()[img2.getIndex(x, y)];
            if (p1.rgbRed != p2.rgbRed || p1.rgbGreen != p2.rgbGreen || p1.rgbBlue != p2.rgbBlue) {
                // std::cout << "Pixel mismatch at (" << x << ", " << y << ")" << std::endl;
                return false;
            }
        }
    }
    return true;
}


/**
 * @brief Test fixture for ImD class.
 */
class ImDTest : public ::testing::Test {
protected:
    ImD default_img; // For tests with a default object
    ImD small_img;   // Small image for some tests
    ImD img_2x1;     // 2x1 image
    ImD img_3x3;     // 3x3 image for filter

    const std::string temp_bmp_filename = "temp_test_image.bmp";

    /**
     * @brief Sets up the test fixture.
     * This method is called before each test.
     */
    void SetUp() override {
        // Create small_img: 2x2, red square
        std::vector<RGBQUAD> small_pixels = {
            {0, 0, 255}, {0, 0, 255}, // R, R
            {0, 0, 255}, {0, 0, 255}  // R, R
        };
        small_img = createTestImD(2, 2, small_pixels);

        // Create img_2x1: {Blue, Green}
        std::vector<RGBQUAD> pixels_2x1 = { {255,0,0}, {0,255,0} }; // B, G
        img_2x1 = createTestImD(2, 1, pixels_2x1);


        std::vector<RGBQUAD> pixels_3x3 = {
            {0,0,0}, {0,0,50}, {0,0,100},
            {0,50,0}, {0,50,50}, {0,50,100},
            {0,100,0}, {0,100,50}, {0,100,100}
        };
        img_3x3 = createTestImD(3, 3, pixels_3x3);
    }

    /**
     * @brief Tears down the test fixture.
     * This method is called after each test.
     */
    void TearDown() override {
        // Remove temporary files if they were created
        std::remove(temp_bmp_filename.c_str());
    }
};

// --- Constructor Tests ---
TEST_F(ImDTest, DefaultConstructor) {
    ASSERT_EQ(default_img.getWidth(), 0);
    ASSERT_EQ(default_img.getHeight(), 0);
    ASSERT_EQ(default_img.getPixels_all(), nullptr);
    // Default header checks, if important
    ASSERT_EQ(default_img.getInfoHeader().biSize, 0); 
}

TEST_F(ImDTest, ParameterizedConstructor) {
    ASSERT_EQ(small_img.getWidth(), 2);
    ASSERT_EQ(small_img.getHeight(), 2);
    ASSERT_NE(small_img.getPixels_all(), nullptr);
    if (small_img.getPixels_all()) {
        RGBQUAD pixel = small_img.getPixels_all()[small_img.getIndex(0,0)];
        ASSERT_EQ(pixel.rgbRed, 255);
        ASSERT_EQ(pixel.rgbGreen, 0);
        ASSERT_EQ(pixel.rgbBlue, 0);
    }
}

TEST_F(ImDTest, CopyConstructor) {
    ImD copy_small_img(small_img);
    ASSERT_TRUE(areImagesEqual(small_img, copy_small_img));

    // Check for deep copy (modifying copy does not affect original)
    if (copy_small_img.getPixels_all() && copy_small_img.getWidth() > 0 && copy_small_img.getHeight() > 0) {
          copy_small_img.getPixels_all()[copy_small_img.getIndex(0,0)] = {10, 20, 30};
    }
    ASSERT_FALSE(areImagesEqual(small_img, copy_small_img));

    // Check that pixel pointers are different (if both are not nullptr)
    if (small_img.getPixels_all() && copy_small_img.getPixels_all()) {
        ASSERT_NE(small_img.getPixels_all(), copy_small_img.getPixels_all());
    }
}

TEST_F(ImDTest, CopyConstructorEmptyImage) {
    ImD empty_img;
    ImD copy_empty_img(empty_img);
    ASSERT_TRUE(areImagesEqual(empty_img, copy_empty_img));
    ASSERT_EQ(copy_empty_img.getPixels_all(), nullptr);
}


// --- Assignment Operator Tests ---
TEST_F(ImDTest, CopyAssignmentOperator) {
    ImD assigned_img;
    assigned_img = small_img; // Assignment
    ASSERT_TRUE(areImagesEqual(small_img, assigned_img));

    // Check for deep copy
    if (assigned_img.getPixels_all() && assigned_img.getWidth() > 0 && assigned_img.getHeight() > 0) {
        assigned_img.getPixels_all()[assigned_img.getIndex(0,0)] = {10,20,30};
    }
    ASSERT_FALSE(areImagesEqual(small_img, assigned_img));
    if (small_img.getPixels_all() && assigned_img.getPixels_all()) {
          ASSERT_NE(small_img.getPixels_all(), assigned_img.getPixels_all());
    }

    // Self-assignment
    assigned_img = small_img; // Revert to original state
    ImD& ref_to_assigned = assigned_img;
    // NOLINTNEXTLINE(clang-diagnostic-self-assign-overloaded)
    assigned_img = ref_to_assigned; // or assigned_img = assigned_img;
    ASSERT_TRUE(areImagesEqual(small_img, assigned_img)); // Should remain as small_img
}

TEST_F(ImDTest, CopyAssignmentToEmpty) {
    ImD empty_img;
    empty_img = small_img;
    ASSERT_TRUE(areImagesEqual(small_img, empty_img));
}

TEST_F(ImDTest, CopyAssignmentFromEmpty) {
    ImD img_to_clear = small_img;
    ImD empty_img;
    img_to_clear = empty_img;
    ASSERT_TRUE(areImagesEqual(empty_img, img_to_clear));
    ASSERT_EQ(img_to_clear.getPixels_all(), nullptr);
}

// --- Accessor Method Tests ---
TEST_F(ImDTest, GettersAndSetters) {
    ImD img;
    ASSERT_EQ(img.getWidth(), 0);
    ASSERT_EQ(img.getHeight(), 0);

    img.setWidth(100);
    img.setHeight(50);
    ASSERT_EQ(img.getWidth(), 100);
    ASSERT_EQ(img.getHeight(), 50);


    ImD img_for_index = createTestImD(10, 5);
    ASSERT_EQ(img_for_index.getIndex(0, 0), 0);
    ASSERT_EQ(img_for_index.getIndex(9, 0), 9);
    ASSERT_EQ(img_for_index.getIndex(0, 1), 10);
    ASSERT_EQ(img_for_index.getIndex(5, 2), 25); // 2 * 10 + 5

    // setPixel_1, setPixel_all, getPixels_all, setPixels_null
    // These tests require more careful setup of pixel data
    ImD pixel_img = createTestImD(1,1);
    ASSERT_NE(pixel_img.getPixels_all(), nullptr);
    
    RGBQUAD new_pixel_val = {10,20,30};
    pixel_img.setPixel_1(0, new_pixel_val);
    if(pixel_img.getPixels_all()){
        ASSERT_EQ(pixel_img.getPixels_all()[0].rgbBlue, 10);
        ASSERT_EQ(pixel_img.getPixels_all()[0].rgbGreen, 20);
        ASSERT_EQ(pixel_img.getPixels_all()[0].rgbRed, 30);
    }

    pixel_img.setPixels_null();
    ASSERT_EQ(pixel_img.getPixels_all(), nullptr);

    RGBQUAD* new_data = new RGBQUAD[1];
    new_data[0] = {40,50,60};
    pixel_img.setPixel_all(new_data); // ImD now owns new_data
    ASSERT_EQ(pixel_img.getPixels_all(), new_data); // Pointers should match
    if(pixel_img.getPixels_all()){
        ASSERT_EQ(pixel_img.getPixels_all()[0].rgbBlue, 40);
    }

}


// -- Gaussian Filter --
TEST_F(ImDTest, ApplyGaussianFilterSequential_3x3) {
    ImD test_img = img_3x3; // Use img_3x3 from SetUp


    test_img.applyGaussianFilter("test_output_gauss");

    ASSERT_EQ(test_img.getWidth(), 3);
    ASSERT_EQ(test_img.getHeight(), 3);
    ASSERT_NE(test_img.getPixels_all(), nullptr);



    if (test_img.getPixels_all()) {
        RGBQUAD center_pixel = test_img.getPixels_all()[test_img.getIndex(1,1)];
        ASSERT_EQ(center_pixel.rgbBlue, 0);   // B
        ASSERT_EQ(center_pixel.rgbGreen, 50); // G
        ASSERT_EQ(center_pixel.rgbRed, 50);   // R
    }
    if (test_img.getPixels_all()) {
        RGBQUAD p00 = test_img.getPixels_all()[test_img.getIndex(0,0)];
        ASSERT_EQ(p00.rgbBlue, 0);
        ASSERT_EQ(p00.rgbGreen, 50);
        ASSERT_EQ(p00.rgbRed, 50);
    }
    std::remove("test_output_gauss_WGaus.bmp");
}

TEST_F(ImDTest, ApplyGaussianFilterParallel_3x3) {
    ImD test_img = img_3x3;
    test_img.applyGaussianFilter_parallel("test_output_gauss_p");

    ASSERT_EQ(test_img.getWidth(), 3);
    ASSERT_EQ(test_img.getHeight(), 3);
    ASSERT_NE(test_img.getPixels_all(), nullptr);

    // Expected result for the central pixel is the same as in the sequential version
    if (test_img.getPixels_all()) {
        RGBQUAD center_pixel = test_img.getPixels_all()[test_img.getIndex(1,1)];
        ASSERT_EQ(center_pixel.rgbBlue, 0);
        ASSERT_EQ(center_pixel.rgbGreen, 50);
        ASSERT_EQ(center_pixel.rgbRed, 50);
    }
    // Border check (border copying logic in parallel version is the same)
    if (test_img.getPixels_all()) {
        RGBQUAD p00 = test_img.getPixels_all()[test_img.getIndex(0,0)];
        ASSERT_EQ(p00.rgbBlue, 0);
        ASSERT_EQ(p00.rgbGreen, 50);
        ASSERT_EQ(p00.rgbRed, 50);
    }
    std::remove("test_output_gauss_p_WGaus_parallel.bmp");
}

TEST_F(ImDTest, ApplyGaussianFilterSmallImage) {
    ImD one_by_one = createTestImD(1,1, {{10,20,30}});
    ImD copy_one_by_one = one_by_one;

    one_by_one.applyGaussianFilter("test_small_gauss");
    // For images < 3x3, the filter should just copy the pixels
    ASSERT_TRUE(areImagesEqual(copy_one_by_one, one_by_one));
    std::remove("test_small_gauss_WGaus.bmp");

    one_by_one = copy_one_by_one; // Restore
    one_by_one.applyGaussianFilter_parallel("test_small_gauss_p");
    ASSERT_TRUE(areImagesEqual(copy_one_by_one, one_by_one));
    std::remove("test_small_gauss_p_WGaus_parallel.bmp");
}

TEST_F(ImDTest, ApplyGaussianFilterToNullPixels) {
    ImD img_null_pixels_valid_dims = createTestImD(10, 10);
    img_null_pixels_valid_dims.setPixels_null();
    img_null_pixels_valid_dims.applyGaussianFilter("test_output_gauss_null");
    ASSERT_EQ(img_null_pixels_valid_dims.getPixels_all(), nullptr); 
    std::remove("test_output_gauss_null_WGaus.bmp");
}



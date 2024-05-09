#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "utils.h"

// Function make the image grayscale
std::vector<std::vector<int>> makeImageGrayScale(unsigned char* img, int width, int height, int channels) {
    std::vector<std::vector<int>> image(height, std::vector<int>(width));
    #pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int gray = 0;
            for (int c = 0; c < channels; c++) {
                gray += img[(y * width + x) * channels + c];
            }
            image[y][x] = gray / channels;
        }
    }
    return image;
}

// Function to create a Gaussian kernel
std::vector<std::vector<double>> createGaussianKernel(int kernelSize, double sigma) {
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;
    int halfSize = kernelSize / 2;
    #pragma omp parallel for reduction(+:sum)
    for (int x = -halfSize; x <= halfSize; x++) {
        for (int y = -halfSize; y <= halfSize; y++) {
            kernel[x + halfSize][y + halfSize] = exp(-(x*x + y*y) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            sum += kernel[x + halfSize][y + halfSize];
        }
    }

    // Normalize the kernel
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

// Function to apply the convolution
std::vector<std::vector<int>> applyConvolution(std::vector<std::vector<int>> &image, std::vector<std::vector<double>> &kernel,  int height_from = 0, int height_to = 0) {
    int height = image.size();
    int width = image[0].size();
    int kernelSize = kernel.size();
    int halfSize = kernelSize / 2;

    if (height_to == 0) {
        height_to = height;
    }
    if (height_from == 0) {
        height_from = 0;
    }
    // Create a new image to store the result
    std::vector<std::vector<int>> result((height_to - height_from), std::vector<int>(width));
    std::cout << "In Convolution Height: " << height_from <<" -> " << height_to << std::endl;
    std::cout << "In Convolution Width: " << width << std::endl;

    // Pad the image with zeros
    std::vector<std::vector<int>> paddedImage((height_to - height_from) + 2 * halfSize, std::vector<int>(width + 2 * halfSize));
    std::cout << "Padded Image Height: " << (height_to - height_from) + 2 * halfSize << std::endl;
    int new_height = (height_to - height_from);
    #pragma omp parallel for
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < width; x++) {
            int value = image[y][x];
            paddedImage[y + halfSize][x + halfSize] = value;
        }
    }
    std::cout << "Padded Image Done" << std::endl;

    // Define the block size
    const int blockSize = 16;

    // Apply the convolution
    #pragma omp parallel for
    for (int yy = 0; yy < new_height; yy += blockSize) {
        for (int xx = 0; xx < width; xx += blockSize) {
            for (int y = yy; y < std::min(yy + blockSize, new_height); y++) {
                for (int x = xx; x < std::min(xx + blockSize, width); x++) {
                    double sum = 0;
                    for (int ky = -halfSize; ky <= halfSize; ky++) {
                        for (int kx = -halfSize; kx <= halfSize; kx++) {
                            sum += kernel[ky + halfSize][kx + halfSize] * paddedImage[y + ky + halfSize][x + kx + halfSize];
                        }
                    }
                    result[y][x] = static_cast<int>(sum);
                }
            }
        }
    }
    std::cout << "Convolution Done" << std::endl;

    return result;
}

std::vector<std::vector<int>> subtractImages(std::vector<std::vector<int>>& originalImage, std::vector<std::vector<int>>& lowPassImage, int height_from = 0, int height_to = 0) {
    int height = originalImage.size();
    int width = originalImage[0].size();

    if (height_to == 0) {
        height_to = height;
    }
    if (height_from == 0) {
        height_from = 0;
    }
    std::cout <<"Subtraction Started" << std::endl;
    int new_height = height_to - height_from;
    std::vector<std::vector<int>> highPassImage(new_height, std::vector<int>(width));
    #pragma omp parallel for
    for(int i = 0; i < new_height; i++) {
        for(int j = 0; j < width; j++) {
            int old_value = originalImage[i][j] - lowPassImage[i][j]; 
            
            int new_value = static_cast<int>((old_value / 255.0) * (255 - 127) + 127);
            highPassImage[i][j] = new_value;
        }
    }

    std::cout << "Subtraction Done" << std::endl;
    return highPassImage;
}

int main() {
    // Load the image
    int width, height, channels;
    unsigned char* img = stbi_load("image.jpg", &width, &height, &channels, 0);
    std::cout << "image uploaded" << std::endl;
    
    // Convert the image to grayscale and store it in a 2D vector
    std::vector<std::vector<int>> image = makeImageGrayScale(img, width, height, channels);

    // Create a Gaussian kernel
    std::vector<std::vector<double>> kernel = createGaussianKernel(11, 7.0);

    // Apply the convolution
    std::vector<std::vector<int>> result = applyConvolution(image, kernel);

    // subtract the result from the original image
    result = subtractImages(image, result);
    

    // Convert the result back to an image
    unsigned char* resultImg = new unsigned char[width * height];
    #pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            resultImg[y * width + x] = static_cast<unsigned char>(result[y][x]);
        }
    }
    
    // Save the result image
    stbi_write_jpg("resultopenmp.jpg", width, height, 1, resultImg, 100);
    std::cout << "Result image saved." << std::endl;
    // Free the memory
    stbi_image_free(img);
    delete[] resultImg;

    return 0;
}
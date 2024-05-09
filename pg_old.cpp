#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "utils.h"

// Function make the image grayscale
std::vector<std::vector<int>> makeImageGrayScale(unsigned char* img, int width, int height, int channels) {
    std::vector<std::vector<int>> image(height, std::vector<int>(width));
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
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < width; x++) {
            int value = image[y][x];
            paddedImage[y + halfSize][x + halfSize] = value;
        }
    }
    std::cout << "Padded Image Done" << std::endl;

    // Apply the convolution
    for (int y = 0; y < new_height ; y++) {
        for (int x = 0; x < width; x++) {
            double sum = 0;
            for (int ky = -halfSize; ky <= halfSize; ky++) {
                for (int kx = -halfSize; kx <= halfSize; kx++) {
                    sum += kernel[ky + halfSize][kx + halfSize] * paddedImage[y + ky + halfSize][x + kx + halfSize];
                }
            }
            result[y][x] = static_cast<int>(sum);
        }
    }
    std::cout << "Convolution Done" << std::endl;

    //remove padding
    std::vector<std::vector<int>> result2(new_height, std::vector<int>(width));
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < width; x++) {
            int value = image[y][x];
            paddedImage[y + halfSize][x + halfSize] = value;
        }
    }
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

// Serial version
/*
int main() {
    // Load the image
    int width, height, channels;
    unsigned char* img = stbi_load("image.jpg", &width, &height, &channels, 0);
    std::cout << "image uploaded" << std::endl;
    
    // Convert the image to grayscale and store it in a 2D vector
    std::vector<std::vector<int>> image = makeImageGrayScale(img, width, height, channels);

    // Create a Gaussian kernel
    std::vector<std::vector<double>> kernel = createGaussianKernel(21, 2.0);

    // Apply the convolution
    std::vector<std::vector<int>> result = applyConvolution(image, kernel);

    // subtract the result from the original image
    result = subtractImages(image, result);
    

    // Convert the result back to an image
    unsigned char* resultImg = new unsigned char[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            resultImg[y * width + x] = static_cast<unsigned char>(result[y][x]);
        }
    }
    
    // Save the result image
    stbi_write_jpg("result2.jpg", width, height, 1, resultImg, 100);
    std::cout << "Result image saved." << std::endl;
    // Free the memory
    stbi_image_free(img);
    delete[] resultImg;

    return 0;
}
*/
// mpi version
/*
int main(int argc, char** argv) {
    std::cout << "Program started" << std::endl;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    std::cout << "MPI initialized" << std::endl;

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    std::cout << "Number of processes: " << world_size << std::endl;

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    std::cout << "Rank of this process: " << world_rank << std::endl;

    // Load the image only on the master process
    int width, height, channels;
    unsigned char* img = NULL;
    std::vector<std::vector<int>> image;
    if (world_rank == 0) {
        std::cout << "Loading image on master process" << std::endl;
        img = stbi_load("parallel.jpg", &width, &height, &channels, 0);
        if (img == NULL) {
            std::cout << "Could not load image" << std::endl;
            return -1;
        }
        std::cout << "Image loaded: " << width << "x" << height <<std::endl;
        image = makeImageGrayScale(img, width, height, channels);
        std::cout << "Image converted to grayscale: " << image.size() << image[0].size() << std::endl;
    
    }
    // Broadcast the width and height to all processes
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier( MPI_COMM_WORLD );

    // Calculate the number of rows that each process should handle
    int height_from = world_rank * height / world_size;
    int height_to = (world_rank + 1) * height / world_size;
    int new_height = height_to - height_from;
    std::cout << "Process " << world_rank << " will handle rows " << height_from << " to " << height_to << std::endl;

    MPI_Barrier( MPI_COMM_WORLD );
    // Calculate the number of rows that each process should handle
    std::vector<int> sendcounts(world_size);
    std::vector<int> displs(world_size);
    for (int i = 0; i < world_size; ++i) {
        sendcounts[i] = (i + 1) * height / world_size - i * height / world_size;
        displs[i] = i * height / world_size;
        std::cout << "Process " << i << " will handle rows " << displs[i] << " to " << displs[i] + sendcounts[i] << std::endl;
    }

    // Flatten the image vector
    std::vector<int> flat_image;
    if (world_rank == 0) {
        for (const auto &row : image) {
            flat_image.insert(flat_image.end(), row.begin(), row.end());
        }
    }

    // Create a buffer to hold the chunk of the image that this process will handle
    std::vector<int> chunk(new_height * width);
    MPI_Barrier( MPI_COMM_WORLD );

    // If this is the root process
    if (world_rank == 0) {
        // Send data to each process
        for (int i = 1; i < world_size; ++i) {
            MPI_Send(flat_image.data() + displs[i]*width, sendcounts[i] * width, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        // Copy its own data
        std::copy(flat_image.begin(), flat_image.begin() + sendcounts[0] * width, chunk.begin());
    } else {
    // If this is not the root process, receive data from the root process
    MPI_Recv(chunk.data(), new_height * width, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    MPI_Barrier( MPI_COMM_WORLD );
    
    // Reshape the chunk vector into a two-dimensional vector
    std::vector<std::vector<int>> chunk_2d(new_height, std::vector<int>(width));
    for (int i = 0; i < chunk.size(); ++i) {
        if(chunk[i] == 0){
            std::cout << "Error: chunk 0"<< std::endl;
        }
        chunk_2d[i / width][i % width] = chunk[i];
    }
    MPI_Barrier( MPI_COMM_WORLD );


    // Each process applies the convolution to its chunk of the image
    std::vector<std::vector<double>> kernel = createGaussianKernel(11, 7.0);
    std::vector<std::vector<int>> result = applyConvolution(chunk_2d, kernel, height_from, height_to);
    

    MPI_Barrier( MPI_COMM_WORLD );

    result = subtractImages(chunk_2d, result, height_from, height_to);
    
    unsigned char* partialresultImg = new unsigned char[width * new_height];
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < width; x++) {
            partialresultImg[y * width + x] = static_cast<unsigned char>(result[y][x]);
        }
    }
    std::string filename = "ppp" + std::to_string(world_rank) + ".jpg";
    stbi_write_jpg(filename.c_str(), width, new_height, 1, partialresultImg, 100);

    // Flatten the result vector
    std::vector<int> flat_result;
    for (const auto &row : result) {
        flat_result.insert(flat_result.end(), row.begin(), row.end());
    }
    std::cout << "flatten image -> " << flat_result.size() << std::endl;
    MPI_Barrier( MPI_COMM_WORLD );
    
    // Create a buffer on the root process to hold the gathered data
    std::vector<int> gathered_data;
    std::cout << "Gathering data: " << gathered_data.size()<< std::endl;

    // If this is the root process
    if (world_rank == 0) {
        // Add its own data first
        gathered_data.insert(gathered_data.end(), flat_result.begin(), flat_result.end());

        // Then receive data from each process
        for (int i = 1; i < world_size; ++i) {
            std::vector<int> recv_data(new_height * width);
            MPI_Recv(recv_data.data(), new_height * width, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gathered_data.insert(gathered_data.end(), recv_data.begin(), recv_data.end());
            std::cout << "Received data from process " << i << " " << gathered_data.back() <<std::endl;
            
        }
    } else {
        // If this is not the root process, send its data to the root process
        if (flat_result.size() > 0) {
            MPI_Send(flat_result.data(), flat_result.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
        } else {
            std::cout << "Process " << world_rank << " has no data to send." << std::endl;
        }
    }
    MPI_Barrier( MPI_COMM_WORLD );

    // The master process saves the result image
    if (world_rank == 0) {
        // Reshape the chunk vector into a two-dimensional vector
        std::vector<std::vector<int>> chunk_2d_result(height, std::vector<int>(width));
        for (int i = 0; i < gathered_data.size(); ++i) {
            if (i / width >= height) {
                break;
                std::cout << "Error: " << i << " " << i / width << " " << height << std::endl;
            }
            if (gathered_data[i] == 0) {
                std::cout << "Error: gathered_data 0"<< std::endl;
            }
            chunk_2d_result[i / width][i % width] = gathered_data[i];
        }
        std::cout << "Saving result image on master process" << std::endl;
        unsigned char* resultImg = new unsigned char[width * height];
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                resultImg[y * width + x] = static_cast<unsigned char>(chunk_2d_result[y][x]);
            }
        }
        stbi_write_jpg("ppp.jpg", width, height, 1, resultImg, 100);
        std::cout << "Result image saved." << std::endl;
        stbi_image_free(img);
        delete[] resultImg;
    }
    MPI_Barrier( MPI_COMM_WORLD );
    // Finalize the MPI environment
    MPI_Finalize();
    std::cout << "MPI finalized" << std::endl;

    return 0;
}
*/

#include <seam.hpp>

#include <memory>
#include <chrono>
#include <cassert>

namespace algorithm {
    
int WORKGROUP_SIZE = 1;

namespace  {

/**
 * @brief Calculates number power of two, which is euqual or bigger than "n"
 */
size_t nextPowerOf2(size_t n)
{
    unsigned count = 0;
    if (n && !(n & (n - 1))) {
        return n;
    }

    while(n != 0) {
        n >>= 1;
        count += 1;
    }
    return 1 << count;
}

void grayImage(cl::Kernel &grayFilter, cl::CommandQueue &queue, cl::Buffer &image, cl::Buffer &result, 
               int height, int width, int normalised_size)  {
    int res = grayFilter.setArg(0, image);
    res = grayFilter.setArg(1, result);
    res = grayFilter.setArg(2, height);
    res = grayFilter.setArg(3, width);
    res = grayFilter.setArg(4, normalised_size);
    res = queue.enqueueNDRangeKernel(grayFilter, cl::NullRange,
            cl::NDRange(width, height),
            cl::NDRange(1, 1));
    assert(res == 0);
}

void sobelImage(cl::Kernel &sobelOperator, cl::CommandQueue &queue, cl::Buffer &image, cl::Buffer &result, 
               int height, int width, int normalised_size)  {
    int res = sobelOperator.setArg(0, image);
    res = sobelOperator.setArg(1, result);
    res = sobelOperator.setArg(2, height);
    res = sobelOperator.setArg(3, width);
    res = sobelOperator.setArg(4, normalised_size);
    res = queue.enqueueNDRangeKernel(sobelOperator, cl::NullRange,
            cl::NDRange(width, height),
            cl::NDRange(1, 1));
    assert(res == 0);
}

void findSeam(cl::Kernel &findSeam, cl::CommandQueue &queue, cl::Buffer &image, cl::Buffer &directions, cl::Buffer &seam,
               int height, int width, int normalised_size)  {
    int res = findSeam.setArg(0, image);
    res = findSeam.setArg(1, height);
    res = findSeam.setArg(2, width);
    res = findSeam.setArg(3, normalised_size);
    res = findSeam.setArg(4, sizeof(float)*normalised_size, nullptr);
    res = findSeam.setArg(5, sizeof(float)*normalised_size, nullptr);
    res = findSeam.setArg(6, directions);
    res = findSeam.setArg(7, seam);
    res = findSeam.setArg(4, normalised_size);
    res = queue.enqueueNDRangeKernel(findSeam, cl::NullRange,
            cl::NDRange(WORKGROUP_SIZE),
            cl::NDRange(WORKGROUP_SIZE));
    assert(res == 0);
}

void removeSeam(cl::Kernel &removeSeam, cl::CommandQueue &queue, cl::Buffer &mask, cl::Buffer &image, cl::Buffer seam,
               int height, int width, int normalised_size)  {
    int res = removeSeam.setArg(0, mask);
    res = removeSeam.setArg(1, image);
    res = removeSeam.setArg(2, height);
    res = removeSeam.setArg(3, width);
    res = removeSeam.setArg(4, normalised_size);
    res = removeSeam.setArg(5, seam);
    res = queue.enqueueNDRangeKernel(removeSeam, cl::NullRange,
            cl::NDRange(WORKGROUP_SIZE),
            cl::NDRange(WORKGROUP_SIZE));
    assert(res == 0);
}

void insertSeam(cl::Kernel &insertSeam, cl::CommandQueue &queue, cl::Buffer &image, cl::Buffer &mask, cl::Buffer seam,
               int height, int width, int normalised_size)  {
    int res = insertSeam.setArg(0, image);
    res = insertSeam.setArg(1, mask);
    res = insertSeam.setArg(2, height);
    res = insertSeam.setArg(3, width);
    res = insertSeam.setArg(4, normalised_size);
    res = insertSeam.setArg(5, seam);
    res = insertSeam.setArg(6, sizeof(float)*normalised_size, nullptr);
    res = queue.enqueueNDRangeKernel(insertSeam, cl::NullRange,
            cl::NDRange(WORKGROUP_SIZE),
            cl::NDRange(WORKGROUP_SIZE));
    assert(res == 0);
}

void rotateImage(cl::Kernel &rotateImage, cl::CommandQueue &queue, cl::Buffer &image, cl::Buffer &result, int direction,
               int height, int width, int normalised_size)  {
    int res = rotateImage.setArg(0, image);
    res = rotateImage.setArg(1, result);
    res = rotateImage.setArg(2, height);
    res = rotateImage.setArg(3, width);
    res = rotateImage.setArg(4, normalised_size);
    res = rotateImage.setArg(5, direction);
    res = queue.enqueueNDRangeKernel(rotateImage, cl::NullRange,
            cl::NDRange(width, height),
            cl::NDRange(1, 1));
    assert(res == 0);
}

}

void Algorithm::resizeBMPImage(image::Image<image::BMPImage, image::BMPColor> &img,
                           int cut_width,
                           int cut_height,
                           const device::Params &params,
                           const execution::Params &e_params)
{
    /// Only for benchmark, may be removed
    auto start = std::chrono::high_resolution_clock::now();

    /// Max workgroup size
    WORKGROUP_SIZE = std::min<int>(params.device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>(), 256);
    int image_width = img.width();
    int image_height = img.height();

    int normalised_width = nextPowerOf2(std::max(image_width-cut_width, image_width));
    int normalised_height = nextPowerOf2(std::max(image_height-cut_height, image_height));
    int normalised_size = std::max(normalised_width, normalised_height);
    
    /// Contains pixels in float type
    float *image = new float [normalised_size * normalised_size];

    /// Converts input image to float* alternative
    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            auto color = img.pixel(j, i);
            image[i*normalised_size + j] = ((size_t) color.red())*256*256 + ((size_t) color.green())*256 + ((size_t) color.blue());
        }
    }
    
    /// Function initialisation
    cl::Kernel verticalSeam;
    if (e_params.advanced) {
        verticalSeam = cl::Kernel(*params.program, "verticalSeamRight");
    } else {
        verticalSeam = cl::Kernel(*params.program, "verticalSeam");
    }
    cl::Kernel removeVerticalSeam = cl::Kernel(*params.program, "removeVerticalSeam");
    cl::Kernel horisontalSeam = cl::Kernel(*params.program, "horisontalSeam");
    cl::Kernel removeHorisontalSeam = cl::Kernel(*params.program, "removeHorisontalSeam");
    cl::Kernel grayFilter = cl::Kernel(*params.program, "grayLevel");
    cl::Kernel sobelOperator = cl::Kernel(*params.program, "sobelOperator");
    cl::Kernel imageRotate = cl::Kernel(*params.program, "rotateImage");
    cl::Kernel insertVerticalSeam = cl::Kernel(*params.program, "insertVerticalSeam");
    cl::Kernel resetBarrier = cl::Kernel(*params.program, "resetBarrier");
    
    /// Buffer creation
    cl::Buffer mask_buffer(params.context, CL_MEM_READ_WRITE, sizeof(float) * normalised_size * normalised_size);
    cl::Buffer image_buffer(params.context, CL_MEM_READ_WRITE, sizeof(float) * normalised_size * normalised_size);
    cl::Buffer temp_buffer(params.context, CL_MEM_READ_WRITE, sizeof(float) * normalised_size * normalised_size);
    cl::Buffer directions_buffer(params.context, CL_MEM_READ_WRITE, sizeof(int) * normalised_size * normalised_size);
    cl::Buffer barrier_buffer(params.context, CL_MEM_READ_WRITE, sizeof(float) * normalised_size * normalised_size);
    cl::Buffer seam_points_buffer(params.context, CL_MEM_READ_WRITE, sizeof(size_t) * normalised_size);
    
    /// Result code
    int res;
    /// Load image to buffer
    res = params.queue->enqueueWriteBuffer(image_buffer, CL_TRUE, 0, sizeof(float) * normalised_size * normalised_size, image);

    /// Apply gray filter
    grayImage(grayFilter, *params.queue, image_buffer, temp_buffer, image_height, image_width, normalised_size);

    /// Apply Sobel operator
    sobelImage(sobelOperator, *params.queue, temp_buffer, mask_buffer, image_height, image_width, normalised_size);

    /// Vertical
    /// Remove seams
    for (int i = 0; i < cut_width; ++i) {
        findSeam(verticalSeam, *params.queue, mask_buffer, directions_buffer, seam_points_buffer, image_height, image_width - i, normalised_size);
        removeSeam(removeVerticalSeam, *params.queue, mask_buffer, image_buffer, seam_points_buffer, image_height, image_width - i, normalised_size);
    }
    
    /// Insert seams
    for (int i = 0; i < -cut_width; ++i) {
        findSeam(verticalSeam, *params.queue, mask_buffer, directions_buffer, seam_points_buffer, image_height, image_width + i, normalised_size);
        insertSeam(insertVerticalSeam, *params.queue, image_buffer, mask_buffer, seam_points_buffer, image_height, image_width + i, normalised_size);
    }
    
    /// Reduce width after changing image
    image_width -= cut_width;
    
    /// Performing rotation
    /// image -> temp
    rotateImage(imageRotate, *params.queue, image_buffer, temp_buffer, 1, image_height, image_width, normalised_size);
    
    /// Rotate image sizes
    std::swap(image_width, image_height);
    
    /// Finds new mask
    /// Apply gray filter
    grayImage(grayFilter, *params.queue, temp_buffer, image_buffer, image_height, image_width, normalised_size);

    /// Apply Sobel operator
    sobelImage(sobelOperator, *params.queue, image_buffer, mask_buffer, image_height, image_width, normalised_size);
    
    
    /// Horisontal
    /// Remove seams
    for (int i = 0; i < cut_height; ++i) {
        findSeam(verticalSeam, *params.queue, mask_buffer, directions_buffer, seam_points_buffer, image_height, image_width - i, normalised_size);
        removeSeam(removeVerticalSeam, *params.queue, mask_buffer, temp_buffer, seam_points_buffer, image_height, image_width - i, normalised_size);
    }
    
    /// Insert seams
    for (int i = 0; i < -cut_height; ++i) {
        findSeam(verticalSeam, *params.queue, mask_buffer, directions_buffer, seam_points_buffer, image_height, image_width + i, normalised_size);
        insertSeam(insertVerticalSeam, *params.queue, temp_buffer, mask_buffer, seam_points_buffer, image_height, image_width + i, normalised_size);
    }
    
    /// Reduce width (height of original image) after changing image
    image_width -= cut_height;
    
    /// Rotate again to normal
    /// temp -> image
    rotateImage(imageRotate, *params.queue, temp_buffer, image_buffer, 0, image_height, image_width, normalised_size);
    
    /// Rotate image sizes
    std::swap(image_width, image_height);


    /// Load image from buffer
    res = params.queue->enqueueReadBuffer(image_buffer, CL_TRUE, 0, sizeof(float)* normalised_size * normalised_size, image);
    params.queue->finish();

    /// Convert float* back to bitmap
    size_t index = 0;
    if (image_width*image_height > img.width()*img.height()) {
        delete[] img.getImage().data;
        img.getImage().data = new image::BMPColor [image_width*image_height];
    }
    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            img.getImage().data[index] = image::BMPColor(((size_t) image[i*normalised_size + j])/(256*256),
                    (((size_t) image[i*normalised_size + j])/256)%256,
                    ((size_t) image[i*normalised_size + j])%256);
            index++;
        }
    }

    /// Set new properties of image
    img.getImage().img_width = image_width;
    img.getImage().img_height = image_height;

    auto finish = std::chrono::high_resolution_clock::now();
    delete[] image;
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s"<<std::endl;
}
}

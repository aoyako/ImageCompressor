#include <seam.hpp>

#include <memory>
#include <chrono>

namespace algorithm {

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

}

void Algorithm::resizeBMPImage(image::Image<image::BMPImage, image::BMPColor> &img,
                           size_t cut_width,
                           size_t cut_height,
                           const device::Params &params)
{
    /// Only for benchmark, may be removed
    auto start = std::chrono::high_resolution_clock::now();

    /// Max workgroup size
    int WORKGROUP_SIZE = std::min<int>(params.device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>(), 256);
    int image_width = img.width();
    int image_height = img.height();

    int normalised_width = nextPowerOf2(image_width);
    int normalised_height = nextPowerOf2(image_height);
    
    /// Contains pixels in float type
    float *image = new float [normalised_width * normalised_height];

    /// Converts input image to float* alternative
    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            auto color = img.pixel(j, i);
            image[i*normalised_width + j] = ((size_t) color.red())*256*256 + ((size_t) color.green())*256 + ((size_t) color.blue());
        }
    }
    
    /// Function initialisation
    cl::Kernel verticalSeam = cl::Kernel(*params.program, "verticalSeam");
    cl::Kernel removeVerticalSeam = cl::Kernel(*params.program, "removeVerticalSeam");
    cl::Kernel horisontalSeam = cl::Kernel(*params.program, "horisontalSeam");
    cl::Kernel removeHorisontalSeam = cl::Kernel(*params.program, "removeHorisontalSeam");
    cl::Kernel grayFilter = cl::Kernel(*params.program, "grayLevel");
    cl::Kernel sobelOperator = cl::Kernel(*params.program, "sobelOperator");
    
    /// Buffer creation
    cl::Buffer mask_buffer(params.context, CL_MEM_READ_WRITE, sizeof(float) * normalised_height * normalised_width);
    cl::Buffer image_buffer(params.context, CL_MEM_READ_WRITE, sizeof(float) * normalised_height * normalised_width);
    cl::Buffer temp_buffer(params.context, CL_MEM_READ_WRITE, sizeof(float) * normalised_height * normalised_width);
    cl::Buffer directions_buffer(params.context, CL_MEM_READ_WRITE, sizeof(int) * normalised_height * normalised_width);
    cl::Buffer seam_points_buffer(params.context, CL_MEM_READ_WRITE, sizeof(size_t) * std::max(normalised_width, normalised_height));
    
    /// Result code
    int res;
    res = params.queue->enqueueWriteBuffer(image_buffer, CL_TRUE, 0, sizeof(float) * normalised_width * normalised_height, image);

    /// Apply gray filter
    res = grayFilter.setArg(0, image_buffer);
    res = grayFilter.setArg(1, temp_buffer);
    res = grayFilter.setArg(2, image_height);
    res = grayFilter.setArg(3, image_width);
    res = grayFilter.setArg(4, normalised_width);
    res = params.queue->enqueueNDRangeKernel(grayFilter, cl::NullRange,  // kernel, offset
            cl::NDRange(normalised_width, normalised_height), // global number of work items
            cl::NDRange(16, 16));

    /// Apply Sobel operator
    res = sobelOperator.setArg(0, temp_buffer);
    res = sobelOperator.setArg(1, mask_buffer);
    res = sobelOperator.setArg(2, image_height);
    res = sobelOperator.setArg(3, image_width);
    res = sobelOperator.setArg(4, normalised_width);
    res = params.queue->enqueueNDRangeKernel(sobelOperator, cl::NullRange,  // kernel, offset
            cl::NDRange(normalised_width, normalised_height), // global number of work items
            cl::NDRange(16, 16));

    /// Search and remove of vertical seams
    res = verticalSeam.setArg(0, mask_buffer);
    res = verticalSeam.setArg(1, image_height);
    res = verticalSeam.setArg(3, normalised_width);
    res = verticalSeam.setArg(6, directions_buffer);
    res = verticalSeam.setArg(7, seam_points_buffer);

    res = removeVerticalSeam.setArg(0, mask_buffer);
    res = removeVerticalSeam.setArg(1, image_buffer);
    res = removeVerticalSeam.setArg(2, image_height);
    res = removeVerticalSeam.setArg(4, normalised_width);
    res = removeVerticalSeam.setArg(5, seam_points_buffer);

    for (int i = 0; i < cut_width; ++i) {
        res = verticalSeam.setArg(2, image_width-i);
        res = verticalSeam.setArg(4, sizeof(float)*image_width, nullptr);
        res = verticalSeam.setArg(5, sizeof(float)*image_width, nullptr);
        res = params.queue->enqueueNDRangeKernel(verticalSeam, cl::NullRange,
                cl::NDRange(WORKGROUP_SIZE),
                cl::NDRange(WORKGROUP_SIZE));


        res = removeVerticalSeam.setArg(3, image_width-i);
        res = params.queue->enqueueNDRangeKernel(removeVerticalSeam, cl::NullRange,
                cl::NDRange(WORKGROUP_SIZE),
                cl::NDRange(WORKGROUP_SIZE));
    }

    /// Search and remove of horisonatal seams
    /// TODO: Optimisation image_width -> image_width-cut_width
    res = horisontalSeam.setArg(0, mask_buffer);
    res = horisontalSeam.setArg(2, image_width);
    res = horisontalSeam.setArg(3, normalised_width);
    res = horisontalSeam.setArg(6, directions_buffer);
    res = horisontalSeam.setArg(7, seam_points_buffer);

    res = removeHorisontalSeam.setArg(0, mask_buffer);
    res = removeHorisontalSeam.setArg(1, image_buffer);
    res = removeHorisontalSeam.setArg(4, normalised_width);
    res = removeHorisontalSeam.setArg(3, image_width);
    res = removeHorisontalSeam.setArg(5, seam_points_buffer);

    for (int i = 0; i < cut_height; ++i) {
        res = horisontalSeam.setArg(1, image_height-i);
        res = horisontalSeam.setArg(4, sizeof(float)*image_height, nullptr);
        res = horisontalSeam.setArg(5, sizeof(float)*image_height, nullptr);
        res = params.queue->enqueueNDRangeKernel(horisontalSeam, cl::NullRange,  // kernel, offset
                cl::NDRange(WORKGROUP_SIZE), // global number of work items
                cl::NDRange(WORKGROUP_SIZE));               // local number (per group)


        res = removeHorisontalSeam.setArg(2, image_height-i);
        res = params.queue->enqueueNDRangeKernel(removeHorisontalSeam, cl::NullRange,
                cl::NDRange(WORKGROUP_SIZE),
                cl::NDRange(WORKGROUP_SIZE));
    }

    /// Read results
    res = params.queue->enqueueReadBuffer(image_buffer, CL_TRUE, 0, sizeof(float)* normalised_width * normalised_height, image);
    params.queue->finish();

    /// Convert float* back to bitmap
    size_t index = 0;
    for (int i = 0; i < image_height-cut_height; ++i) {
        for (int j = 0; j < image_width-cut_width; ++j) {
            img.getImage().data[index] = image::BMPColor(((size_t) image[i*normalised_width + j])/(256*256),
                    (((size_t) image[i*normalised_width + j])/256)%256,
                    ((size_t) image[i*normalised_width + j])%256);
            index++;
        }
    }

    /// Set new properties of image
    img.getImage().img_width = image_width-cut_width;
    img.getImage().img_height = image_height-cut_height;

    auto finish = std::chrono::high_resolution_clock::now();
    delete[] image;
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s"<<std::endl;
}

}

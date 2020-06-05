#pragma once 

#include <limits>
#include <iostream>
#include <memory>
#ifdef __APPLE__
    #include <OpenCL/cl.hpp>
#else
    #include <CL/cl.hpp>
#endif

#include <BMP/bmp_image.hpp>
#include <image.hpp>

namespace device {
    /**
     * @brief Struct contain information about hardware where program will be executed
     */
    struct Params {
        std::shared_ptr<cl::CommandQueue> queue;
        cl::Device device;
        cl::Context context;
        std::shared_ptr<cl::Program> program;
        cl::Platform platform;
    };
} // namespace device

namespace execution {
    /**
     * @brief Params stores additional algorithm properties
     */
    class Params {
    public:
        bool advanced;
    };
} // namespace execution

namespace algorithm {
    /**
     * @brief Wrapper around resize funciton
     */
    class Algorithm {
    public:
        static void resizeBMPImage(image::Image<image::BMPImage, image::BMPColor> &img,
                                   int cut_width,
                                   int cut_height,
                                   const device::Params &params,
                                   const execution::Params &e_params
                                  );
    private:
        size_t _previous_width;
        size_t _previous_height;
    };
    
namespace utils {
    /**
    * @brief Calculates number power of two, which is euqual or bigger than "n"
    */
    size_t nextPowerOf2(size_t n);

} // namespace utils

} // namespace algorithm

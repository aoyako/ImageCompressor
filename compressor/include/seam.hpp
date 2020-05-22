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
}

namespace algorithm {
    /**
     * @brief Wrapper around resize funciton
     */
    class Algorithm {
    public:
        static void resizeBMPImage(image::Image<image::BMPImage, image::BMPColor> &img,
                                   size_t cut_width,
                                   size_t cut_height,
                                   const device::Params &params
                                  );
    private:
        size_t previous_width;
        size_t previous_height;
    };
    
}

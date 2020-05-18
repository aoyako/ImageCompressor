#include <seam.hpp>

#include <memory>

namespace algorithm {

void Seamer::resizeBMPImage(image::Image<image::BMPImage, image::BMPColor> &img,
                           size_t cut_width,
                           size_t cut_height, image::Image<image::BMPImage, image::BMPColor> &img_mask, const Params &param,
                           std::shared_ptr<cl::CommandQueue> queue, std::shared_ptr<cl::Context> context, std::shared_ptr<cl::Program> program)
{
    int WORKGROUP_SIZE = 256;
    int image_width = img.width();
    int image_height = img.height();
    
    size_t *image = new size_t [image_width * image_height];
    size_t *mask = new size_t [image_width * image_height];
    for (int i = 0; i < image_width * image_height; ++i) {
        auto color = img.pixel(i%image_width, i/image_width);
        image[i] = ((size_t) color.red())*256*256 + ((size_t) color.green())*256 + ((size_t) color.blue());
        mask[i] = img_mask.pixel(i%image_width, i/image_width).red();
    }
    
    size_t *points = new size_t [image_width];
    
    cl::Kernel vs = cl::Kernel(*program, "verticalSeam");
    cl::Kernel vsr = cl::Kernel(*program, "removeVerticalSeam");
    
    cl::Buffer mask_buffer(*context, CL_MEM_READ_WRITE, sizeof(size_t) * image_width * image_height);
    cl::Buffer image_buffer(*context, CL_MEM_READ_WRITE, sizeof(size_t) * image_width * image_height);
    cl::Buffer directions_buffer(*context, CL_MEM_READ_WRITE, sizeof(int) * image_width * image_height);
    cl::Buffer seam_points_buffer(*context, CL_MEM_READ_WRITE, sizeof(size_t) * std::max(image_width, image_height));
    
    int res;
    res = queue->enqueueWriteBuffer(mask_buffer, CL_TRUE, 0, sizeof(size_t) * image_width * image_height, mask);
    res = queue->enqueueWriteBuffer(image_buffer, CL_TRUE, 0, sizeof(size_t) * image_width * image_height, image);

    for (int i = 0; i < cut_width; ++i) {
        res = vs.setArg(0, mask_buffer);
        res = vs.setArg(1, image_height);
        res = vs.setArg(2, image_width-i);
        res = vs.setArg(3, image_width);
        res = vs.setArg(4, sizeof(size_t)*std::max(image_width, image_height), nullptr);
        res = vs.setArg(5, sizeof(size_t)*std::max(image_width, image_height), nullptr);
        res = vs.setArg(6, directions_buffer);
        res = vs.setArg(7, seam_points_buffer);
        res = queue->enqueueNDRangeKernel(vs, cl::NullRange,  // kernel, offset
                cl::NDRange(WORKGROUP_SIZE), // global number of work items
                cl::NDRange(WORKGROUP_SIZE));               // local number (per group)


        res = vsr.setArg(0, mask_buffer);
        res = vsr.setArg(1, image_buffer);
        res = vsr.setArg(2, image_height);
        res = vsr.setArg(3, image_width-i);
        res = vsr.setArg(4, image_width);
        res = vsr.setArg(5, seam_points_buffer);
        res = queue->enqueueNDRangeKernel(vsr, cl::NullRange,
                cl::NDRange(WORKGROUP_SIZE),
                cl::NDRange(WORKGROUP_SIZE));

//        res = queue->finish();
    }
    res = queue->enqueueReadBuffer(image_buffer, CL_TRUE, 0, sizeof(size_t)* image_width * image_height, image);
    queue->finish();

    size_t index = 0;
    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width-cut_width; ++j) {
            img.getImage().data[index] = image::BMPColor(image[i*image_width + j]/(256*256), (image[i*image_width + j]/256)%256, image[i*image_width + j]%256);
            index++;
        }
    }
    img.getImage().img_width = image_width-cut_width;
}

}

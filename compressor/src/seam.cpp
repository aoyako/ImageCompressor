#include <seam.hpp>

namespace algorithm {

void Seamer::resizeBMPImage(image::Image<image::BMPImage, image::BMPColor> &img,
                           size_t cut_width, size_t width_iterations,
                           size_t cut_height, size_t height_iterations, image::Image<image::BMPImage, image::BMPColor> &mask, const Params &param)
{
    size_t *seam_image = new size_t [img.width()*img.height()];
    for (size_t i = 0; i < img.width()*img.height(); ++i) {
        seam_image[i] = 0;
    }
    for (size_t iter = 0; iter < width_iterations; ++iter) {
        auto points = Seamer::createVerticalSeams(mask, cut_width, seam_image, param);
        std::sort(points.begin(), points.end(), std::greater<size_t>());
        img.getImage().updateSide(mask.width()-cut_width, image::BMPImage::DIRECTION::HORISONTAL, points);
        mask.getImage().updateSide(mask.width()-cut_width, image::BMPImage::DIRECTION::HORISONTAL, points);
    }

    for (size_t iter = 0; iter < height_iterations; ++iter) {
        auto points = Seamer::createHorisontalSeams(mask, cut_height, seam_image, param);
        std::sort(points.begin(), points.end(), std::greater<size_t>());
        img.getImage().updateSide(mask.height()-cut_height, image::BMPImage::DIRECTION::VERTICAL, points);
        mask.getImage().updateSide(mask.height()-cut_height, image::BMPImage::DIRECTION::VERTICAL, points);
    }
}

}

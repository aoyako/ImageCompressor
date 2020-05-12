#include <image_adapter.hpp>

ImageWrapper::ImageWrapper(QImage *img)
{
    image = img;
}

ImageWrapper::operator image::Image<image::BMPImage, image::BMPColor>() const
{
    image::BMPColor *data = new image::BMPColor [image->width()*image->height()];
    for (int x = 0; x < image->width(); ++x) {
        for (int y = 0; y < image->height(); ++y) {
            QColor pixel = image->pixel(x, y);
            data[y + x*image->height()] = image::BMPColor(pixel.red(), pixel.green(), pixel.blue());
        }
    }
    return image::BMPImage(image->width(), image->height(), data);
}

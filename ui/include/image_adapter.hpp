#pragma once

#include <QImage>

#include <seam.hpp>

/*
 * Class for converting QImage to bitmap image for algorithm
 */
class ImageWrapper
{
public:
    ImageWrapper(QImage *img);
    ImageWrapper(image::BMPImage *img);

    operator image::Image<image::BMPImage, image::BMPColor>() const;
    operator QImage() const;
private:
    QImage *qimage;
    image::BMPImage *bimage;
};

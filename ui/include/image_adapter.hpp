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
    operator image::Image<image::BMPImage, image::BMPColor>() const;
private:
    QImage* image;
};

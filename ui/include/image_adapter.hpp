#pragma once

#include <QImage>

#include <seam.hpp>

/**
 * @brief Class for converting QImage to bitmap image for algorithm
 */
class ImageWrapper
{
public:
    ImageWrapper(QImage *img);
    ImageWrapper(image::BMPImage *img);

    /**
     * @brief Converts QImage to bitmap
     */
    operator image::Image<image::BMPImage, image::BMPColor>() const;

    /**
     * @brief Converts bitmap to QImage
     */
    operator QImage() const;

private:
    /**
     * @brief Stored QImage
     */
    QImage *qimage;

    /**
     * @brief Stored bitmap
     */
    image::BMPImage *bimage;
};

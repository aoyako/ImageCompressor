#include <QPixmap>

#include <image_adapter.hpp>

ImageWrapper::ImageWrapper(QImage *img)
{
    qimage = img;
}

ImageWrapper::ImageWrapper(image::BMPImage *img)
{
    bimage = img;
}

ImageWrapper::operator image::Image<image::BMPImage, image::BMPColor>() const
{
    image::BMPColor *data = new image::BMPColor [qimage->width()*qimage->height()];
    for (int x = 0; x < qimage->width(); ++x) {
        for (int y = 0; y < qimage->height(); ++y) {
            QColor pixel = qimage->pixel(x, y);
            data[(qimage->height()-y-1)*qimage->width() + x] = image::BMPColor(pixel.red(), pixel.green(), pixel.blue());
        }
    }
    return image::BMPImage(qimage->width(), qimage->height(), data);
}

ImageWrapper::operator QImage() const
{
    QImage image = QPixmap(bimage->width(), bimage->height()).toImage();
    for (int x = 0; x < bimage->width(); ++x) {
        for (int y = 0; y < bimage->height(); ++y) {
            image::BMPColor pixel = bimage->pixel(x, y);
            QColor col(pixel.red(), pixel.green(), pixel.blue());
            image.setPixel(x, bimage->height()-y-1, col.rgba());
        }
    }
    return image;
}

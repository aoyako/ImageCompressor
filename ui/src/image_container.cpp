#include <image_container.hpp>

void ImageContainer::setImage(const QImage &img) {
    image = img;
}

void ImageContainer::setBackup() {
    _backup_image = image;
}

void ImageContainer::backup() {
    image = _backup_image;
}

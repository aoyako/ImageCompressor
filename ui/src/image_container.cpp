#include <image_container.hpp>

void ImageContainer::setImage(const QImage &img) {
    image = img;
}

void ImageContainer::setBackup() {
    backup_image = image;
}

void ImageContainer::backup() {
    image = backup_image;
}

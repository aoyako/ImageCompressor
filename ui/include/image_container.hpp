#pragma once

#include <QImage>

class ImageContainer
{
public:
    void setImage(const QImage &img);
    QImage image;
};

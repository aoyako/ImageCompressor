#pragma once

#include <QImage>

class ImageContainer
{
public:
    void setImage(const QImage &img);
    void setBackup();
    void backup();
    QImage image;
    QImage backup_image;
};

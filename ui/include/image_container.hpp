#pragma once

#include <QImage>

/**
 * @brief Stores image and perform backups
 */
class ImageContainer
{
public:
    /**
     * @brief Sets image to container
     */
    void setImage(const QImage &img);

    /**
     * @brief Saves backup
     */
    void setBackup();

    /**
     * @brief Restore backup
     */
    void backup();

    /**
     * @brief Stored image
     */
    QImage image;

private:
    /**
     * @brief Backup image
     */
    QImage _backup_image;
};

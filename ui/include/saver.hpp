#pragma once

#include <QImage>

/**
 * @brief Performs image saving
 */
class ImageSaver
{
public:
    /**
     * @brief Sets "autosave" property
     */
    void setAutosave(bool state);

    /**
     * @brief Saves image
     */
    void save(const QImage &image);

private:
    /**
     * @brief Autosave flag
     */
    bool autosave;
};

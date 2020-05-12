#pragma once

class ImageSaver
{
public:
    void setAutosave(bool value);
    void save(const QImage &image);
private:
    bool autosave;
};

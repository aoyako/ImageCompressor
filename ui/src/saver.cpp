#include <QFileDialog>

#include <saver.hpp>

void ImageSaver::setAutosave(bool value)
{
    autosave = value;
}

void ImageSaver::save(const QImage &image)
{
    if (autosave) {
        image.save("result.bmp");
    } else {
        QString file_name = QFileDialog::getSaveFileName(nullptr,
            QObject::tr("Save Result Image"), "",
            QObject::tr("Images (*.png *.jpg, *bmp, *jpeg)"));
        image.save(file_name);
    }
}

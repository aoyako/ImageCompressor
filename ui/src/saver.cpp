#include <QFileDialog>

#include <saver.hpp>

void ImageSaver::setAutosave(bool state)
{
    autosave = state;
}

void ImageSaver::save(const QImage &image)
{
    if (autosave) {
        image.save("result.bmp");
    } else {
        QString file_name = QFileDialog::getSaveFileName(nullptr,
            QObject::tr("Save Result Image"), "/home/aoyako/images",
            QObject::tr("Images (*.png *.jpg, *bmp, *jpeg)"));

        if (!file_name.isEmpty() && !file_name.isNull()) {
            image.save(file_name);
        }
    }
}

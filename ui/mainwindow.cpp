#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <image_adapter.hpp>

#include <QImage>
#include <QFileDialog>
#include <seam.hpp>
#include <iostream>
#include <future>
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(size());
    statusBar()->setSizeGripEnabled(false);
    QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ui->progressBar->setVisible(false);
    ui->progressBar->reset();
    ui->img_height->setEnabled(false);
    ui->img_width->setEnabled(false);

    ui->low_color_border->setRange(0, 255);
    ui->low_color_border->setValue(algorithm::Params::DEFAULT_MIN_BORDER_COLOR);
    ui->hight_color_border->setRange(0, 255);
    ui->hight_color_border->setValue(algorithm::Params::DEFAULT_MAX_BORDER_COLOR);

    ui->hight_color_addition->setRange(0, 8000);
    ui->hight_color_addition->setValue(algorithm::Params::DEFAULT_MAX_ADDITION);
    ui->low_color_addition->setRange(0, 8000);
    ui->low_color_addition->setValue(algorithm::Params::DEFAULT_MID_ADDITION);

    ui->new_width->setRange(0, 8000);
    ui->new_height->setRange(0, 8000);

    ui->process->setDisabled(true);

    saver.setAutosave(false);
    ui->checkBox->setDown(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/aoyako/cpp/compressing/build", tr("Image Files (*.png *.jpg *.bmp)"));

    if(!file_name.isEmpty()&& !file_name.isNull()){
        QImage source_image(file_name);
        image.setImage(source_image);
        image.setBackup();
        ui->img_height->setEnabled(true);
        ui->img_width->setEnabled(true);
        ui->img_height->setReadOnly(true);
        ui->img_width->setReadOnly(true);
        ui->img_height->setText(QString::number(source_image.height()));
        ui->img_width->setText(QString::number(source_image.width()));
        ui->new_width->setValue(source_image.width());
        ui->new_height->setValue(source_image.height());

        ui->process->setDisabled(false);
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    saver.setAutosave(arg1);
}

void MainWindow::on_process_clicked()
{
    image::Image<image::BMPImage, image::BMPColor> result = ImageWrapper(&image.image);
//    algorithm::Params params(ui->low_color_border->value(),
//                             ui->hight_color_border->value(),
//                             ui->hight_color_addition->value(),
//                             ui->low_color_addition->value());

//    params.fat_lines = false;s
//    auto mask = algorithm::Algorithm::SobelEdges(
//                algorithm::Algorithm::NoiseRemove(
//                algorithm::Algorithm::GrayLevel(image::Image<image::BMPImage, image::BMPColor>(ImageWrapper(&image.image)), params), params), params);
//    mask = algorithm::Algorithm::DoubleTreshold(mask, algorithm::Params(30, 40, 0, 0));
//    mask.save("/home/aoyako/images/MASK.BMP");

    size_t new_width = ui->new_width->value();
    size_t new_height = ui->new_height->value();
//    std::cout<<new_width<<" "<<new_height<<std::endl;

//    size_t seam_limit = 50;
//    if ((image.image.width()-new_width >= seam_limit) || (image.image.height()-new_height >= seam_limit)) {
//    algorithm::Seamer::resizeBMPImage(result,
//                                      seam_limit, (image.image.width()-new_width) / seam_limit,
//                                      seam_limit, (image.image.height()-new_height) / seam_limit, mask, params);
//    }

//    algorithm::Seamer::resizeBMPImage(result,
//                                      (image.image.width()-new_width) % seam_limit, 1,
//                                      (image.image.height()-new_height) % seam_limit, 1, mask, params);


    algorithm::Params params(30, 120, 500, 100000000);
    auto mask = algorithm::Algorithm::SobelEdges(
                algorithm::Algorithm::GrayLevel(result, params), params);


    auto handle = std::async(std::launch::async, algorithm::Seamer::resizeBMPImage, std::ref(result),
                             1, (image.image.width()-new_width),
                             1, (image.image.height()-new_height), std::ref(mask), std::cref(params));
//    std::async(algorithm::Seamer::resizeBMPImage, std::ref(result),
//                                              1,  (image.image.width()-new_width),
//                                              1, (image.image.height()-new_height), std::move(mask), params);
    handle.get();
    image.setImage(ImageWrapper(&result.getImage()));
    saver.save(image.image);
    image.backup();
}

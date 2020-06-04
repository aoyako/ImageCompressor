#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <image_adapter.hpp>

#include <QImage>
#include <QFileDialog>
#include <seam.hpp>
#include <iostream>
#include <future>
#include <thread>
#include <unistd.h>
#include <sstream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), exec()
{
    ui->setupUi(this);
    setFixedSize(size());
    statusBar()->setSizeGripEnabled(false);
    QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ui->progressBar->setVisible(false);
    ui->progressBar->reset();
    ui->img_height->setEnabled(false);
    ui->img_width->setEnabled(false);

    ui->new_width->setEnabled(false);
    ui->new_height->setEnabled(false);

    ui->process->setDisabled(true);

    saver.setAutosave(false);
    ui->autosave->setDown(false);

    ui->usegpu->setDown(false);
    exec.switchToCPU();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("(*.png *.jpg *.bmp)"));

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

        ui->new_width->setEnabled(true);
        ui->new_height->setEnabled(true);
        ui->new_width->setRange(1, 2*source_image.width());
        ui->new_height->setRange(1, 2*source_image.height());
        ui->new_width->setValue(source_image.width());
        ui->new_height->setValue(source_image.height());

        ui->process->setDisabled(false);
    }
}

void MainWindow::on_process_clicked()
{
    image::Image<image::BMPImage, image::BMPColor> result = ImageWrapper(&image.image);

    size_t new_width = ui->new_width->value();
    size_t new_height = ui->new_height->value();

    auto handle = std::async(std::launch::async, algorithm::Algorithm::resizeBMPImage, std::ref(result),
                             (image.image.width()-new_width),
                             (image.image.height()-new_height), exec.getDeviceParams(), exec.getExecutionParams());

    handle.get();
    image.setImage(ImageWrapper(&result.getImage()));
    saver.save(image.image);
    image.backup();
}

void MainWindow::on_usegpu_stateChanged(int value)
{
    if (value == Qt::Checked) {
        exec.switchToGPU();
    } else {
        exec.switchToCPU();
    }
}

void MainWindow::on_autosave_stateChanged(int value)
{
    if (value == Qt::Checked) {
        saver.setAutosave(true);
    } else {
        saver.setAutosave(false);
    }
}

void MainWindow::on_lines_stateChanged(int value)
{
    if (value == Qt::Checked) {
        exec.setPrettyView(true);
    } else {
        exec.setPrettyView(false);
    }
}

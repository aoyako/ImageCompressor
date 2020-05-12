#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QImage>
#include <QFileDialog>
#include <seam.hpp>

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

    ui->hight_color_addition->setRange(0, 800);
    ui->hight_color_addition->setValue(algorithm::Params::DEFAULT_MAX_ADDITION);
    ui->low_color_addition->setRange(0, 800);
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
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

    if(!file_name.isEmpty()&& !file_name.isNull()){
        QImage source_image(file_name);
        image.setImage(source_image);
        ui->img_height->setEnabled(true);
        ui->img_width->setEnabled(true);
        ui->img_height->setReadOnly(true);
        ui->img_width->setReadOnly(true);
        ui->img_height->setText(QString::number(source_image.height()));
        ui->img_width->setText(QString::number(source_image.width()));
        ui->new_width->setValue(source_image.height());
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
    saver.save(image.image);
}

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

void MainWindow::initDevices(const cl::Platform &platform) {
    devices.push_back(device::Params{});
    devices.back().platform = std::make_shared<cl::Platform>(platform);

    std::vector<cl::Device> all_devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(all_devices.size() == 0){
        std::cerr<<"No devices found for "<<platform.getInfo<CL_PLATFORM_NAME>()<<". Check OpenCL installation!\n";
    }

    devices.back().device = std::make_shared<cl::Device>(all_devices[0]);
    std::cout<<"Found device: "<<devices.back().device->getInfo<CL_DEVICE_NAME>()<<std::endl;

    devices.back().context = std::make_shared<cl::Context>(*devices.back().device);
    cl::Program::Sources sources;

    std::ifstream code("kernel.cl");
    if (!code) {
        std::cout<<"Cannot find \"kernel.cl\" file!"<<std::endl;
    }
    std::string kernel_code;
    {
        std::ostringstream ss;
        ss << code.rdbuf();
        kernel_code = ss.str();
    }

    sources.push_back({kernel_code.c_str(), kernel_code.length()});

    devices.back().program = std::make_shared<cl::Program>(*devices.back().context, sources);
    if (devices.back().program->build({*devices.back().device}) != CL_SUCCESS) {
        std::cout <<"Error building: "<<  devices.back().program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(*devices.back().device) << std::endl;
        exit(1);
    }
    devices.back().queue = std::make_shared<cl::CommandQueue>(*devices.back().context, *devices.back().device);
}

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


    ui->new_width->setRange(0, 8000);
    ui->new_height->setRange(0, 8000);

    ui->process->setDisabled(true);

    saver.setAutosave(false);
    ui->autosave->setDown(false);

    ui->usegpu->setDown(false);

        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);

        if (all_platforms.size() == 0) {
            std::cout<<" No platforms found. Check OpenCL installation!\n";
            exit(1);
        }
        if (all_platforms.size() == 1) {
            ui->usegpu->setDisabled(true);
        }
        for (auto const &platrform : all_platforms) {
            initDevices(platrform);
        }
        device_option = all_platforms.size()-1;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/aoyako/cpp/compressing/build", tr("(*.png *.jpg *.bmp)"));

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

    size_t new_width = ui->new_width->value();
    size_t new_height = ui->new_height->value();

    auto handle = std::async(std::launch::async, algorithm::Seamer::resizeBMPImage, std::ref(result),
                             (image.image.width()-new_width),
                             (image.image.height()-new_height), devices[device_option]);

    handle.get();
    image.setImage(ImageWrapper(&result.getImage()));
    saver.save(image.image);
    image.backup();
}

void MainWindow::on_usegpu_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        device_option = 0;
    } else {
        device_option = devices.size()-1;
    }
}

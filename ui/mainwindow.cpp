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

    // get all platforms (drivers)
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);

        if (all_platforms.size() == 0) {
            std::cout<<" No platforms found. Check OpenCL installation!\n";
            exit(1);
        }
        cl::Platform default_platform=all_platforms[0];

        // get default device (CPUs, GPUs) of the default platform
        std::vector<cl::Device> all_devices;
        default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if(all_devices.size() == 0){
            std::cout<<" No devices found. Check OpenCL installation!\n";
            exit(1);
        }

        // use device[1] because that's a GPU; device[0] is the CPU
        cl::Device default_device=all_devices[0];
        std::cout<< "Using device: "<<default_device.getInfo<CL_DEVICE_NAME>()<<"\n";
        std::cout<<"memory: "<<default_device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()*sizeof(cl_ulong)<<std::endl;
        std::cout<<"groups: "<<default_device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()<<std::endl;

        context = std::make_shared<cl::Context>(default_device);
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

        program = std::make_shared<cl::Program>(*context, sources);
        if (program->build({default_device}) != CL_SUCCESS) {
            std::cout << "Error building: " << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << std::endl;
            exit(1);
        }
        queue = std::make_shared<cl::CommandQueue>(*context, default_device);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/aoyako/cpp/compressing/build", tr("(*.png *.jpg *.bmp)"));

//    QString file_name = "/home/aoyako/images/source3.jpg";
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

    algorithm::Params params(30, 120, 500, 100000000);
    auto mask = algorithm::Algorithm::SobelEdges(
                algorithm::Algorithm::GrayLevel(result, params), params);


    auto handle = std::async(std::launch::async, algorithm::Seamer::resizeBMPImage, std::ref(result),
                             (image.image.width()-new_width),
                             (image.image.height()-new_height), std::ref(mask), std::cref(params), queue, context, program);
//    std::async(algorithm::Seamer::resizeBMPImage, std::ref(result),
//                                              1,  (image.image.width()-new_width),
//                                              1, (image.image.height()-new_height), std::move(mask), params);
    usleep(60000000);
    handle.get();
    image.setImage(ImageWrapper(&result.getImage()));
    saver.save(image.image);
    image.backup();
}

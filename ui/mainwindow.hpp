#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <memory>

#include <image_container.hpp>
#include <saver.hpp>
#include <seam.hpp>

#ifdef __APPLE__
    #include <OpenCL/cl.hpp>
#else
    #include <CL/cl.hpp>
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_checkBox_stateChanged(int arg1);

    void on_process_clicked();

    void on_usegpu_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    ImageSaver saver;
    ImageContainer image;
    void initDevices(const cl::Platform &platform);
    std::vector<device::Params> devices;
    size_t device_option = 0;
};
#endif // MAINWINDOW_HPP

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <memory>

#include <image_container.hpp>
#include <saver.hpp>

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

private:
    Ui::MainWindow *ui;
    ImageSaver saver;
    ImageContainer image;
    std::shared_ptr<cl::CommandQueue> queue;
    std::shared_ptr<cl::Context> context;
    std::shared_ptr<cl::Program> program;
};
#endif // MAINWINDOW_HPP

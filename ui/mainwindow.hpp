#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <memory>
#ifdef __APPLE__
    #include <OpenCL/cl.hpp>
#else
    #include <CL/cl.hpp>
#endif

#include <image_container.hpp>
#include <saver.hpp>
#include <seam.hpp>
#include <execution.hpp>


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
    /**
     * @brief Loads an image
     */
    void on_actionOpen_triggered();

    /**
     * @brief Stars image processing
     */
    void on_process_clicked();

    /**
     * @brief Changes code execution hardware
     */
    void on_usegpu_stateChanged(int value);

    /**
     * @brief Changes "autosave" flag
     */
    void on_autosave_stateChanged(int value);

    /**
     * @brief Changes "algorithm" flag
     */
    void on_lines_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    /**
     * @brief Saver saves image depending on parameters
     */
    ImageSaver saver;

    /**
     * @brief ImageContainer makes image backups
     */
    ImageContainer image;

    /**
     * @brief Changes processing device
     */
    Execution exec;
};
#endif // MAINWINDOW_HPP

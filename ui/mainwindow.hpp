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
#include <QFutureWatcher>
#include <image_adapter.hpp>
#include <QPushButton>

/**
 * @brief Reacts when button is pushed and start the algorithm
 */
class Notifier : public QObject{
    Q_OBJECT
public:
    Notifier(): result(1, 1, nullptr) {}
    int run(image::Image<image::BMPImage, image::BMPColor> &img,
                           int cut_width,
                           int cut_height,
                           const device::Params &params,
                           const execution::Params &e_params) {
        algorithm::Algorithm::resizeBMPImage(img, cut_width, cut_height, params, e_params);
        return 0;
    }
    
    void setImage(QImage *img) {
        result = ImageWrapper(img);
    }
    
    void setSaver(ImageSaver *s) {
        saver = s;
    }
    
    void setButton(QPushButton *b) {
        button = b;
    }
    
    /// Result image
    image::Image<image::BMPImage, image::BMPColor> result;
    /// Pointer to saver class
    ImageSaver *saver;
    /// Pointer to "process" button
    QPushButton *button;
    
public slots:
    /// Is called when algorithm is done
    void finished() {
        button->setEnabled(true);
        saver->save(ImageWrapper(&result.getImage()));
    };
};

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
    
    Notifier n;
    
    QFutureWatcher<int> w;
};

#endif // MAINWINDOW_HPP

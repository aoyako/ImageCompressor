#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <image_container.hpp>
#include <saver.hpp>

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

private:
    Ui::MainWindow *ui;
    ImageSaver saver;
    ImageContainer image;
};
#endif // MAINWINDOW_HPP

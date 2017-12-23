#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTimer>
#include <wiringPiI2C.h>
#include <ads1115.h>
#include <wiringPi.h>
#include <softPwm.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void makePlot();
private:
    Ui::MainWindow *ui;
    qreal timeInterval;
};

#endif // MAINWINDOW_H

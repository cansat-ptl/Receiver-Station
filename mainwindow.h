#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>
#include <QtCharts>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void readSerial();
    void updateData(QString);
    void on_pngbutton1_clicked();
    void on_pngbutton2_clicked();


private:
    Ui::MainWindow *ui;
    QSerialPort *receiver;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;

    QChart *chart_alt;
    QChart *chart_prs;
    QChart *chart_t2;
    QChart *chart_vbat;
    QChart *chart_ax;
    QChart *chart_ay;
    QChart *chart_az;

    QValueAxis *axisX_alt;
    QValueAxis *axisY_alt;
    QValueAxis *axisX_prs;
    QValueAxis *axisY_prs;
    QValueAxis *axisX_t2;
    QValueAxis *axisY_t2;
    QValueAxis *axisX_vbat;
    QValueAxis *axisY_vbat;
    QValueAxis *axisX_ax;
    QValueAxis *axisY_ax;
    QValueAxis *axisX_ay;
    QValueAxis *axisY_ay;
    QValueAxis *axisX_az;
    QValueAxis *axisY_az;

    QLineSeries *series_alt;
    QLineSeries *series_prs;
    QLineSeries *series_t2;
    QLineSeries *series_vbat;
    QLineSeries *series_ax;
    QLineSeries *series_ay;
    QLineSeries *series_az;

    int pngCounter = 0;
    int pngCounter2 = 0;

    int altMax = 0;
    int prsMax = 0;
    int t2Max = 0;
    int vbatMax = 6;
    int altMin = 100000;
    int prsMin = 120000;
    int t2Min = 100;
    int vbatMin = 9;
    double axMax = -20.0;
    double axMin = 20.0;
    double ayMax = -20.0;
    double ayMin = 20.0;
    double azMax = -20.0;
    double azMin = 20.0;
    int etMin1 = 0;
    int etMin2 = 0;
};

#endif // MAINWINDOW_H

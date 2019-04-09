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

    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;
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
};

#endif // MAINWINDOW_H

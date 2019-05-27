#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>




namespace Ui {
class MainWindow;
}

class ChartView : public QChartView
{
public:
    ChartView(QChart *chart, QWidget *parent = 0);

protected:
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    bool m_isTouching;
};

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
    void writeToTerminal(QString);


    void on_reconnectButtonR_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *receiver;
    QSerialPort *antenna;
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

    ChartView *chartview_alt;
    ChartView *chartview_prs;
    ChartView *chartview_vbat;
    ChartView *chartview_t2;
    ChartView *chartview_ax;
    ChartView *chartview_ay;
    ChartView *chartview_az;

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

    double latStation = 0;
    double lonStation = 0;
    double altStation = 95;
    double altBar = 0;
    double altGPS = 0;
    double altMax = -1;
    double prsMax = -1;
    double t2Max = -1;
    double vbatMax = -1;
    double altMin = 9999999;
    double prsMin = 9999999;
    double t2Min = 9999999;
    double vbatMin = 9999999;
    double axMax = -1;
    double axMin = 9999999;
    double ayMax = -1;
    double ayMin = 9999999;
    double azMax = -1;
    double azMin = 9999999;
    int etMin1 = 0;
    int etMin2 = 0;

    bool mainCreated = false;
    bool orientCreated = false;
};



#endif // MAINWINDOW_H

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


    void on_reconnectButtonR_clicked();

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

    double latStation = 62.03389;
    double lonStation = 129.73306;
    double altStation = 95;
    double altBar = 0;
    double altGPS = 0;
    int altMax = -1;
    int prsMax = -1;
    int t2Max = -1;
    double vbatMax = -1;
    int altMin = INT_MAX;
    int prsMin = INT_MAX;
    int t2Min = INT_MAX;
    double vbatMin = 999999;
    double axMax = -1;
    double axMin = 999999;
    double ayMax = -1;
    double ayMin = 999999;
    double azMax = -1;
    double azMin = 999999;
    int etMin1 = 0;
    int etMin2 = 0;

    bool mainCreated = false;
    bool orientCreated = false;
};



#endif // MAINWINDOW_H

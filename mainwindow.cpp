#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QtCharts>
#include <QtGui/QMouseEvent>
#include <QSettings>
#include <QVariant>

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent),
    m_isTouching(false)
{
    setRubberBand(QChartView::RectangleRubberBand);
}

bool ChartView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        m_isTouching = true;
        chart()->setAnimationOptions(QChart::NoAnimation);
    }
    return QChartView::viewportEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isTouching)
        m_isTouching = false;

    // Because we disabled animations when touch event was detected
    // we must put them back on.
    chart()->setAnimationOptions(QChart::SeriesAnimations);

    QChartView::mouseReleaseEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-30, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(30, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui -> setupUi(this);

    // Creating charts for altitude, pressure, outside temperature (t2) and battery voltage (vbat)
    chart_alt = new QChart;
    chart_prs = new QChart;
    chart_t2 = new QChart;
    chart_vbat = new QChart;
    chart_ax = new QChart;
    chart_ay = new QChart;
    chart_az = new QChart;

    chartview_alt = new ChartView(chart_alt);
    chartview_prs = new ChartView(chart_prs);
    chartview_vbat = new ChartView(chart_vbat);
    chartview_t2 = new ChartView(chart_t2);
    chartview_ax = new ChartView(chart_ax);
    chartview_ay = new ChartView(chart_ay);
    chartview_az = new ChartView(chart_az);

    ui -> mainLayout -> addWidget(chartview_alt, 0, 0);
    ui -> mainLayout -> addWidget(chartview_prs, 0, 1);
    ui -> mainLayout -> addWidget(chartview_vbat, 1, 0);
    ui -> mainLayout -> addWidget(chartview_t2,1, 1);
    ui -> orientLayout -> addWidget(chartview_ax);
    ui -> orientLayout -> addWidget(chartview_ay);
    ui -> orientLayout -> addWidget(chartview_az);


    // Creating QLineSeries containers for charts
    series_alt = new QLineSeries;
    series_prs = new QLineSeries;
    series_t2 = new QLineSeries;
    series_vbat = new QLineSeries;
    series_ax = new QLineSeries;
    series_ay = new QLineSeries;
    series_az = new QLineSeries;

    // ui -> xyzchart -> setChart(chart_xyz);

    // Altitude chart setting
    chart_alt -> legend() -> hide(); // hide legend
    chart_alt -> addSeries(series_alt); // connecting with QLineSeries
    chart_alt -> setTitle("Altitude chart"); // title of chart
    // Altitude chart axis setting
    axisX_alt = new QValueAxis; // creating a QValueAxis class for X axis of altitude chart
    axisX_alt -> setLabelFormat("%i"); // setting a value axis label format
    axisX_alt -> setTitleText("Seconds"); // title of value axis
    chart_alt -> addAxis(axisX_alt, Qt::AlignBottom); // connecting X value axis with altitude chart
    series_alt -> attachAxis(axisX_alt); // connecting QLineSeries container with X value axis

    axisY_alt = new QValueAxis; // creating a QValueAxis class for Y axis of altitude chart
    axisY_alt -> setLabelFormat("%i"); // setting a value axis label format
    axisY_alt -> setTitleText("Meters"); // title of value axis
    axisY_alt -> setMax(0);
    axisY_alt -> setMin(0);
    chart_alt -> addAxis(axisY_alt, Qt::AlignLeft); // connecting QLineSeries container with Y value axis
    series_alt -> attachAxis(axisY_alt);  // connecting QLineSeries container with Y value axis

    // Pressure chart setting
    chart_prs -> legend() -> hide(); // hide legend
    chart_prs -> addSeries(series_prs); // connecting with QLineSeries
    chart_prs -> setTitle("Pressure chart"); // title of chart
    // Pressure chart axis setting
    axisX_prs = new QValueAxis; // creating a QValueAxis class for X axis of pressure chart
    axisX_prs -> setLabelFormat("%i"); // setting a value axis label format
    axisX_prs -> setTitleText("Seconds"); // title of value axis
    chart_prs -> addAxis(axisX_prs, Qt::AlignBottom); // connecting X value axis with pressure chart
    series_prs -> attachAxis(axisX_prs); // connecting QLineSeries container with X value axis

    axisY_prs = new QValueAxis; // creating a QValueAxis class for Y axis of pressure chart
    axisY_prs -> setLabelFormat("%i"); // setting a value axis label format
    axisY_prs -> setTitleText("kPa"); // title of value axis
    axisY_prs -> setMax(0);
    axisY_prs -> setMin(0);
    chart_prs -> addAxis(axisY_prs, Qt::AlignLeft); // connecting QLineSeries container with X value axis
    series_prs -> attachAxis(axisY_prs); // connecting QLineSeries container with X value axis

    // Next charts have absolutely same settings
    // Outside temperature chart setting
    chart_t2 -> legend() -> hide();
    chart_t2 -> addSeries(series_t2);
    chart_t2 -> setTitle("Outside temperature chart");
    // Outside temperature chart axis setting
    axisX_t2 = new QValueAxis;
    axisX_t2 -> setLabelFormat("%i");
    axisX_t2 -> setTitleText("Seconds");
    chart_t2 -> addAxis(axisX_t2, Qt::AlignBottom);
    series_t2 -> attachAxis(axisX_t2);

    axisY_t2 = new QValueAxis;
    axisY_t2 -> setLabelFormat("%i");
    axisY_t2 -> setTitleText("°С");
    axisY_t2 -> setMax(0);
    axisY_t2 -> setMin(0);
    chart_t2 -> addAxis(axisY_t2, Qt::AlignLeft);
    series_t2 -> attachAxis(axisY_t2);

    // Battery voltage chart setting
    chart_vbat -> legend() -> hide();
    chart_vbat -> addSeries(series_vbat);
    chart_vbat -> setTitle("Battery voltage chart");
    // Battery voltage chart axis setting
    axisX_vbat = new QValueAxis;
    axisX_vbat -> setLabelFormat("%i");
    axisX_vbat -> setTickCount(1);
    axisX_vbat -> setTitleText("Seconds");
    chart_vbat -> addAxis(axisX_vbat, Qt::AlignBottom);
    series_vbat -> attachAxis(axisX_vbat);

    axisY_vbat = new QValueAxis;
    axisY_vbat -> setLabelFormat("%.2f");
    axisY_vbat -> setTitleText("Volts");
    axisY_vbat -> setMax(0);
    axisY_vbat -> setMin(0);
    chart_vbat -> addAxis(axisY_vbat, Qt::AlignLeft);
    series_vbat -> attachAxis(axisY_vbat);

    //AX chart setting
    chart_ax -> legend() -> hide();
    chart_ax -> addSeries(series_ax);
    chart_ax -> setTitle("Acceleration by X chart");
    // AX chart axis setting
    axisX_ax = new QValueAxis;
    axisX_ax -> setLabelFormat("%i");
    axisX_ax -> setTickCount(1);
    axisX_ax -> setTitleText("Seconds");
    chart_ax -> addAxis(axisX_ax, Qt::AlignBottom);
    series_ax -> attachAxis(axisX_ax);

    axisY_ax = new QValueAxis;
    axisY_ax -> setLabelFormat("%.2f");
    axisY_ax -> setTitleText("m/s²");
    axisY_ax -> setMax(0);
    axisY_ax -> setMin(0);
    chart_ax -> addAxis(axisY_ax, Qt::AlignLeft);
    series_ax -> attachAxis(axisY_ax);

    //AY chart setting
    chart_ay -> legend() -> hide();
    chart_ay -> addSeries(series_ay);
    chart_ay -> setTitle("Acceleration by Y chart");
    // AX chart axis setting
    axisX_ay = new QValueAxis;
    axisX_ay -> setLabelFormat("%i");
    axisX_ay -> setTickCount(1);
    axisX_ay -> setTitleText("Seconds");
    chart_ay -> addAxis(axisX_ay, Qt::AlignBottom);
    series_ay -> attachAxis(axisX_ay);

    axisY_ay = new QValueAxis;
    axisY_ay -> setLabelFormat("%.2f");
    axisY_ay -> setTitleText("m/s²");
    axisY_ay -> setMax(0);
    axisY_ay -> setMin(0);
    chart_ay -> addAxis(axisY_ay, Qt::AlignLeft);
    series_ay -> attachAxis(axisY_ay);

    //AZ chart setting
    chart_az -> legend() -> hide();
    chart_az -> addSeries(series_az);
    chart_az -> setTitle("Acceleration by Z chart");
    // AZ chart axis setting
    axisX_az = new QValueAxis;
    axisX_az -> setLabelFormat("%i");
    axisX_az -> setTickCount(1);
    axisX_az -> setTitleText("Seconds");
    chart_az -> addAxis(axisX_az, Qt::AlignBottom);
    series_az -> attachAxis(axisX_az);

    axisY_az = new QValueAxis;
    axisY_az -> setLabelFormat("%.2f");
    axisY_az -> setTitleText("m/s²");
    axisY_az -> setMax(0);
    axisY_az -> setMin(0);
    chart_az -> addAxis(axisY_az, Qt::AlignLeft);
    series_az -> attachAxis(axisY_az);

    ui -> orient_terminal -> setReadOnly(1);
    ui -> main_terminal -> setReadOnly(1);
    ui -> gps_terminal -> setReadOnly(1);

    QPalette pMain = ui -> main_terminal -> palette();
    pMain.setColor(QPalette::Base, Qt::black);
    pMain.setColor(QPalette::Text, Qt::green);
    ui -> main_terminal -> setPalette(pMain);

    QPalette pOrient = ui -> orient_terminal -> palette();
    pOrient.setColor(QPalette::Base, Qt::black);
    pOrient.setColor(QPalette::Text, Qt::green);
    ui -> orient_terminal -> setPalette(pOrient);

    QPalette pGPS = ui -> gps_terminal -> palette();
    pGPS.setColor(QPalette::Base, Qt::black);
    pGPS.setColor(QPalette::Text, Qt::green);
    ui -> gps_terminal -> setPalette(pGPS);


    // COM port settings

    QString portNameR , baudRateR , portNameA, baudRateA;
    QSettings sett("settings.ini", QSettings::IniFormat);
    sett.beginGroup("MAIN");
    portNameR = sett.value("COM_NAME_R", "COM7").toString();
    baudRateR = sett.value("BAUD_RATE_R", "9600").toString();
    latStation = sett.value("STATION_LATITUDE", "62.03389").toDouble();
    lonStation = sett.value("STATION_LONGITUDE", "129.73306").toDouble();
    portNameA = sett.value("COM_NAME_A", "COM3").toString();
    baudRateA = sett.value("BAUD_RATE_A", "9600").toString();
    sett.endGroup();

    receiver = new QSerialPort(this); // creating a object of receiver using as radio input
    serialBuffer = ""; // buffer for data packets
    parsed_data = ""; // handled version of data packets

    receiver -> setPortName(portNameR);
    receiver -> open(QSerialPort::ReadOnly);
    receiver -> setBaudRate(baudRateR.toInt());
    receiver -> setDataBits(QSerialPort::Data8);
    receiver -> setFlowControl(QSerialPort::NoFlowControl);
    receiver -> setParity(QSerialPort::NoParity);
    receiver -> setStopBits(QSerialPort::OneStop);
    receiver -> flush();
    QObject::connect(receiver, SIGNAL(readyRead()), this, SLOT(readSerial()));

    antenna = new QSerialPort(this);

    antenna -> setPortName(portNameA);
    antenna -> open(QSerialPort::WriteOnly);
    antenna -> setBaudRate(baudRateA.toInt());
    antenna -> setDataBits(QSerialPort::Data8);
    antenna -> setFlowControl(QSerialPort::NoFlowControl);
    antenna -> setParity(QSerialPort::NoParity);
    antenna -> setStopBits(QSerialPort::OneStop);
    antenna -> flush();

}

void MainWindow::on_reconnectButtonR_clicked()
{
    receiver -> close();
    receiver -> open(QSerialPort::ReadOnly);
    Sleep(uint(2000));
}


MainWindow::~MainWindow()
{
    receiver -> close();
    delete ui;
}

// Reading data from serial port
void MainWindow::readSerial()
{
    QStringList buffer_split = serialBuffer.split("\n");
    if(buffer_split.length() < 2)
    {
        serialData = receiver -> readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }
    else
    {
        serialBuffer = "";
        parsed_data = buffer_split[0];
        QFile dataPackets(".\\CSVs\\data_packets.csv");
        dataPackets.open(QFile::WriteOnly|QFile::Append);
        QTextStream packetStream(&dataPackets);
        packetStream << parsed_data << "\n";
        dataPackets.close();
        MainWindow::updateData(parsed_data);
    }
}

void MainWindow::writeToTerminal(QString angles)
{
    angles.append( "\r");
    QByteArray ba = angles.toLatin1();
    antenna->write(ba);
}

// Updating data on UI
void MainWindow::updateData(QString s)
{
    int l = s.length();
    QString temp = "";
    QString type = "";
    for (int j = 0; j < l; j++)
    {
        if (s[j] == ':')
        {
            if (s[j-1] == '5')
            {
                if (s[j-2] == 'T')
                {
                    while (s[j + 1] != ':')
                    {
                        type += s[j + 1];
                        j++;
                    }
                }
            }
        }
    }


    if (type == "ORIENT")
    {
        int n = 0, et = 0, rawAx = 0, rawAy = 0, rawAz = 0, i = 0;
        bool damaged[5] = {false};

        for (i = 0; i < l; i++)
        {
            if (s[i] == '=')
            {
                if (s[i - 1] == 'N')
                {
                    while (s[i + 1] != ';')
                    {
                        temp += s[i + 1];
                        i++;
                        if (temp.length() > 10)
                        {
                            damaged[0] = true;
                            break;
                        }
                    }
                    n = temp.toInt();
                    temp = "";
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'T')
                {
                    if (s[i - 2] == 'E')
                    {
                        while (s[i + 1] != ';')
                        {
                            temp += s[i + 1];
                            i++;
                            if (temp.length() > 10)
                            {
                                damaged[1] = true;
                                break;
                            }
                        }
                        et = temp.toInt();
                        temp = "";
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'X')
                {
                    if (s[i - 2] == 'A')
                    {
                        while (s[i + 1] != ';')
                        {
                            temp += s[i + 1];
                            i++;
                            if (temp.length() > 10)
                            {
                                damaged[2] = true;
                                break;
                            }
                        }
                        rawAx = temp.toInt();
                        temp = "";
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'Y')
                {
                    if (s[i - 2] == 'A')
                    {
                        while (s[i + 1] != ';')
                        {
                            temp += s[i + 1];
                            i++;
                            if (temp.length() > 10)
                            {
                                damaged[3] = true;
                                break;
                            }
                        }
                        rawAy = temp.toInt();
                        temp = "";
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'Z')
                {
                    if (s[i - 2] == 'A')
                    {
                        while (s[i + 1] != ';')
                        {
                            temp += s[i + 1];
                            i++;
                            if (temp.length() > 10)
                            {
                                damaged[4] = true;
                                break;
                            }
                        }
                        rawAz = temp.toInt();
                        temp = "";
                    }
                }
            }
        }

        double ax = (double(rawAx) / 10.0) * 9.81, ay = (double(rawAy) / 10.0) * 9.81, az = (double(rawAz) / 10.0) * 9.81;

        if (!damaged[0] && !damaged[1] && !damaged[2] && !damaged[3] && !damaged[4])
        {
            ui -> statusBar -> showMessage("Пакет ориентации №" + QString::number(n) + " получен удачно");
            ui -> orient_terminal -> append(s.trimmed() + "\t OK");
        }
        else
        {
            ui -> statusBar -> showMessage("Последний пакет принят с повреждениями, данные могут быть неточны");
            ui -> orient_terminal -> append(s.trimmed() + "\t DAMAGED");
        }

        QFile dataOrient(".\\CSVs\\data_orient.csv");
        dataOrient.open(QFile::WriteOnly|QFile::Append);
        QTextStream orientStream(&dataOrient);

        if (!damaged[0])
        {
            ui -> orient_nvalue -> setText(QString::number(n));
            orientStream << n << ",";
        }
        if (!damaged[1])
        {
            ui -> orient_etvalue -> setText(QString::number(et) + " s");
            orientStream << et << ",";
        }
        if (!damaged[2])
        {
            ui -> orient_axvalue -> setText(QString::number(ax) + " m/s²");
            orientStream << ax << ",";
        }
        if (!damaged[3])
        {
            ui -> orient_ayvalue -> setText(QString::number(ay) + " m/s²");
            orientStream << ay << ",";
        }
        if (!damaged[4])
        {
            ui -> orient_azvalue -> setText(QString::number(az) + " m/s²");
            orientStream << az << "\n";
        }
        dataOrient.close();

        if (!damaged[1])
        {
            if (et > etMin2)
            {
                etMin2 = INT_MAX;
                axisX_ax -> setMin(et);
                axisX_ay -> setMin(et);
                axisX_az -> setMin(et);
                axisX_ax -> setMax(et + 1);
                axisX_ay -> setMax(et + 1);
                axisX_az -> setMax(et + 1);
            }
            else
            {
                axisX_ax -> setMax(et);
                axisX_ay -> setMax(et);
                axisX_az -> setMax(et);
            }
        }

        if (!damaged[2])
        {
            if (ax > axMax)
            {
                axMax = ax + 1;
                axisY_ax -> setMax(axMax + 1);
            }
            if (ax< axMin)
            {
                axMin = ax - 1;
                axisY_ax -> setMin(axMin - 1);
            }
            series_ax -> append(et , ax);
        }

        if (!damaged[3])
        {
            if (ay > ayMax)
            {
                ayMax = ay + 1;
                axisY_ay -> setMax(ayMax + 1);
            }
            if (ay < ayMin)
            {
                ayMin = ay - 1;
                axisY_ay -> setMin(ayMin - 1);
            }
            series_ay -> append(et , ay);
        }

        if (!damaged[4])
        {
            if (az > azMax)
            {
                azMax = az + 1;
                axisY_az -> setMax(azMax + 1);
            }
            if (az < azMin)
            {
                azMin = az - 1;
                axisY_az -> setMin(azMin - 1);
            }
            series_az -> append(et , az);
        }
    }
    else if (type == "GPS")
    {
        int i = 0, n = 0, et = 0, alt = 0, sat = 0;
        double lat = 0, lon = 0;
        temp = "";
        bool damaged[6] = {false};
            for (i = 0; i < l; i++)
            {
                if (s[i] == '=')
                {
                    if (s[i - 1] == 'N')
                    {
                        if (s[i - 2] != 'O')
                        {
                            while (s[i + 1] != ';')
                            {
                                temp += s[i + 1];
                                i++;
                                if (temp.length() > 10)
                                {
                                    damaged[0] = true;
                                    break;
                                }
                            }
                            n = temp.toInt();
                            temp = "";
                        }
                    }
                }
                if (s[i] == '=')
                {
                    if (s[i - 1] == 'T')
                    {
                        if (s[i - 2] == 'E')
                        {
                            while (s[i + 1] != ';')
                            {
                                temp += s[i + 1];
                                i++;
                                if (temp.length() > 10)
                                {
                                    damaged[1] = true;
                                    break;
                                }
                            }
                            et = temp.toInt();
                            temp = "";
                        }
                    }
                }
                if (s[i] == '=')
                {
                    if (s[i - 1] == 'T')
                    {
                        if (s[i - 2] == 'A')
                        {
                            if (s[i - 3] == 'S')
                            {
                                while (s[i + 1] != ';')
                                {
                                    temp += s[i + 1];
                                    i++;
                                    if (temp.length() > 10)
                                    {
                                        damaged[2] = true;
                                        break;
                                    }
                                }
                                sat = temp.toInt();
                                temp = "";
                            }
                        }
                    }
                }
                if (s[i] == '=')
                {
                    if (s[i - 1] == 'T')
                    {
                        if (s[i - 2] == 'A')
                        {
                            if(s[i - 3] == 'L')
                            {
                                while (s[i + 1] != ';')
                                {
                                    temp += s[i + 1];
                                    i++;
                                    if (temp.length() > 20)
                                    {
                                        damaged[3] = true;
                                        break;
                                    }
                                }
                                lat = temp.toDouble();
                                temp = "";
                            }
                        }
                    }
                }
                if (s[i] == '=')
                {
                    if (s[i - 1] == 'N')
                    {
                        if (s[i - 2] == 'O')
                        {
                            if(s[i - 3] == 'L')
                            {
                                while (s[i + 1] != ';')
                                {
                                    temp += s[i + 1];
                                    i++;
                                    if (temp.length() > 20)
                                    {
                                        damaged[4] = true;
                                        break;
                                    }
                                }
                                lon = temp.toDouble();
                                temp = "";
                            }
                        }
                    }
                }
                if (s[i] == '=')
                {
                    if (s[i - 1] == 'T')
                    {
                        if (s[i - 2] == 'L')
                        {
                            if (s[i - 3] == 'A')
                            {
                                while (s[i + 1] != ';')
                                {
                                    temp += s[i + 1];
                                    i++;
                                    if (temp.length() > 10)
                                    {
                                        damaged[5] = true;
                                        break;
                                    }
                                }
                                alt = temp.toInt();
                                temp = "";
                            }
                        }
                    }
                }
            }
            altGPS = alt;
            if (!damaged[0] && !damaged[1] && !damaged[2] && !damaged[3] && !damaged[4] && !damaged[5])
            {
                ui -> statusBar -> showMessage("Пакет ориентации №" + QString::number(n) + " получен удачно.");
                ui -> gps_terminal -> append(s.trimmed() + "\t OK");
            }
            else
            {
                ui -> statusBar -> showMessage("Последний пакет принят с повреждениями, данные могут быть неточны");
                ui -> gps_terminal -> append(s.trimmed() + "\t OK");
            }
            QFile dataGPS(".\\CSVs\\data_gps.csv");
            dataGPS.open(QFile::WriteOnly|QFile::Append);
            QTextStream gpsStream(&dataGPS);

            // Updating labels in right side of UI
            if (!damaged[0])
            {
                ui -> gps_nvalue -> setText(QString::number(n));
                gpsStream << n << ",";
            }
            if (!damaged[1])
            {
                ui -> gps_etvalue -> setText(QString::number(et) + " s");
                gpsStream << et << ",";
            }
            if (!damaged[2])
            {
                ui -> gps_latvalue -> setText(QString::number(lat) + " °");
                gpsStream << lat << ",";
            }
            if (!damaged[3])
            {
                ui -> gps_lonvalue -> setText(QString::number(lon) + " °");
                gpsStream << lon << ",";
            }
            if (!damaged[4])
            {
                ui -> gps_satvalue -> setText(QString::number(sat));
                gpsStream << sat << ",";
            }
            if (!damaged[5])
            {
                ui -> gps_altvalue -> setText(QString::number(alt) + " m");
                gpsStream << alt << "\n";
            }
            dataGPS.close();

            // Calculating antenna angles
            double r = 6371200.0 + ((altBar + altGPS) / 2);
            double x1 = r * cos(lat) * cos(lon);
            double y1 = r * cos(lat) * sin(lon);
            double z1 = (altBar + altGPS) / 2;
            double x2 = r * cos(latStation) * cos(lonStation);
            double y2 = r * cos(latStation) * sin(lonStation);
            double z2 = altStation;
            double alpha = atan(abs(y2 - y1) / abs(x2 - x1)) * 180/M_PI;
            double hypot = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
            double beta = atan(abs(z2 - z1) / hypot) * 180/M_PI;
            ui -> angle_alpha -> setText(QString::number(alpha));
            ui -> angle_beta -> setText(QString::number(beta));
            int rndAlpha = int(alpha);
            int rndBeta = int(beta);
            writeToTerminal("A=" + QString::number(rndAlpha) + ";" + "B=" + QString::number(rndBeta) + ";");
    }
    else if (type == "MAIN")
    {
        int n = 0, et = 0, vbatRaw = 0, altRaw = 0, prsRaw = 0, t1Raw = 0, t2Raw = 0, i = 0;
        bool damaged[7] = {false};
        temp = "";
        for (i = 0; i < l; i++)
        {
            if (s[i] == '=')
            {
                if (s[i - 1] == 'N')
                {
                    while (s[i + 1] != ';')
                    {
                        temp += s[i + 1];
                        i++;
                        if (temp.length() > 10)
                        {
                            damaged[0] = true;
                            break;
                        }
                    }
                    n = temp.toInt();
                    temp = "";
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'T')
                {
                    if (s[i - 2] == 'E')
                    {
                        while (s[i + 1] != ';')
                        {
                            temp += s[i + 1];
                            i++;
                            if (temp.length() > 10)
                            {
                                damaged[1] = true;
                                break;
                            }
                        }
                        et = temp.toInt();
                        temp = "";
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'T')
                {
                    if (s[i - 2] == 'A')
                    {
                        if (s[i - 3] == 'B')
                        {
                            if (s[i - 4] == 'V')
                            {
                                while (s[i + 1] != ';')
                                {
                                    temp += s[i + 1];
                                    i++;
                                    if (temp.length() > 10)
                                    {
                                        damaged[2] = true;
                                        break;
                                    }
                                }
                                vbatRaw = temp.toInt();
                                temp = "";
                            }
                        }
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'T')
                {
                    if (s[i - 2] == 'L')
                    {
                        if(s[i - 3] == 'A')
                        {
                            while (s[i + 1] != ';')
                            {
                                temp += s[i + 1];
                                i++;
                                if (temp.length() > 10)
                                {
                                    damaged[3] = true;
                                    break;
                                }
                            }
                            altRaw = temp.toInt();
                            temp = "";
                        }
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'S')
                {
                    if (s[i - 2] == 'R')
                    {
                        if(s[i - 3] == 'P')
                        {
                            while (s[i + 1] != ';')
                            {
                                temp += s[i + 1];
                                i++;
                                if (temp.length() > 10)
                                {
                                    damaged[4] = true;
                                    break;
                                }
                            }
                            prsRaw = temp.toInt();
                            temp = "";
                        }
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == '1')
                {
                    if (s[i - 2] == 'T')
                    {
                        while (s[i + 1] != ';')
                        {
                            temp += s[i + 1];
                            i++;
                            if (temp.length() > 10)
                            {
                                damaged[5] = true;
                                break;
                            }
                        }
                        t1Raw = temp.toInt();
                        temp = "";
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == '2')
                {
                    if (s[i - 2] == 'T')
                    {
                        while (s[i + 1] != ';')
                        {
                            temp += s[i + 1];
                            i++;
                            if (temp.length() > 10)
                            {
                                damaged[6] = true;
                                break;
                            }
                        }
                        t2Raw = temp.toInt();
                        temp = "";
                    }
                }
            }
        }
        double alt = double(altRaw)/10.0, t1 = double(t1Raw)/10.0, t2 = double(t2Raw)/10.0;
        altBar = alt;
        double vbat = double(vbatRaw) / 10.0;
        int prs = double(prsRaw) / 10000;
        if (!damaged[0] && !damaged[1] && !damaged[2] && !damaged[3] && !damaged[4] && !damaged[5] && !damaged[6])
        {
            ui -> statusBar -> showMessage("Главный пакет №" + QString::number(n) + " получен удачно.");
            ui -> main_terminal -> append( s.trimmed() + "\t OK");
        }
        else
        {
            ui -> statusBar -> showMessage("Последний пакет принят с повреждениями, данные могут быть неточны");
            ui -> main_terminal -> append(s.trimmed() + "\t DAMAGED");
        }
        QFile dataMain(".\\CSVs\\data_main.csv");
        dataMain.open(QFile::WriteOnly|QFile::Append);
        QTextStream mainStream(&dataMain);

        // Updating labels in right side of UI
        if (!damaged[0])
        {
            ui -> nvalue -> setText(QString::number(n));
            mainStream << n << ",";
        }
        if (!damaged[1])
        {
            ui -> etvalue -> setText(QString::number(et) + " s");
            mainStream << et << ",";
        }
        if (!damaged[2])
        {
            ui -> vbatvalue -> setText(QString::number(vbat) + " V");
            mainStream << vbat << ",";
        }
        if (!damaged[3])
        {
            ui -> altvalue -> setText(QString::number(alt) + " m");
            mainStream << alt << ",";
        }
        if (!damaged[4])
        {
            ui -> prsvalue -> setText(QString::number(prs) + " kPa");
            mainStream << prs << ",";
        }
        if (!damaged[5])
        {
            ui -> t1value -> setText(QString::number(t1) + " C°");
            mainStream << t1 << ",";
        }
        if (!damaged[6])
        {
            ui -> t2value -> setText(QString::number(t2) + " C°");
            mainStream << t2 << "\n";
        }
        dataMain.close();

        // et = estimated time
        // Updating charts and X axis of every chart setting new maximum that is ET
        if (!damaged[1])
        {
            if (et > etMin1)
            {
                etMin1 = INT_MAX;
                axisX_alt -> setMin(et);
                axisX_prs -> setMin(et);
                axisX_t2 -> setMin(et);
                axisX_vbat -> setMin(et);
                axisX_alt -> setMax(et + 1);
                axisX_prs -> setMax(et + 1);
                axisX_t2 -> setMax(et + 1);
                axisX_vbat -> setMax(et + 1);
            }
            else
            {
                axisX_alt -> setMax(et);
                axisX_prs -> setMax(et);
                axisX_t2 -> setMax(et);
                axisX_vbat -> setMax(et);
            }
        }

        if (!damaged[3])
        {
            if (alt > altMax)
            {
                altMax = alt + 1;
                axisY_alt -> setMax(altMax + 1);
            }
            if (alt < altMin)
            {
                altMin = alt - 1;
                axisY_alt -> setMin(altMin - 1);
            }
            series_alt -> append(et , alt);
        }

        if (!damaged[4])
        {
            if (prs > prsMax)
            {
                prsMax = prs + 1;
                axisY_prs -> setMax(prsMax + 1);
            }
            if (prs < prsMin)
            {
                prsMin = prs - 1;
                axisY_prs -> setMin(prsMin - 1);
            }
            series_prs -> append(et , prs);
        }

        if (!damaged[6])
        {
            if (t2 > t2Max)
            {
                t2Max = t2 + 1;
                axisY_t2 -> setMax(t2Max + 1);
            }
            if (t2 < t2Min)
            {
                t2Min = t2 - 1;
                axisY_t2 -> setMin(t2Min - 1);
            }
            series_t2 -> append(et , t2);
        }

        if (!damaged[2])
        {
            if (vbat > vbatMax)
            {
                vbatMax = vbat + 1;
                axisY_vbat -> setMax(vbatMax + 1);
            }
            if (vbat < vbatMin)
            {
                vbatMin = vbat - 1;
                axisY_vbat -> setMin(vbatMin - 1);
            }
            series_vbat -> append(et , vbat);
        }
    }
}


void MainWindow::on_pngbutton1_clicked()
{
    QPixmap pAlt = chartview_alt -> grab();
    pAlt.save(".\\Images\\" + QString::number(pngCounter) + " altitude.png","PNG");

    QPixmap pPrs = chartview_prs -> grab();
    pPrs.save(".\\Images\\" + QString::number(pngCounter) + " pressure.png","PNG");

    QPixmap pT2 = chartview_t2 -> grab();
    pT2.save(".\\Images\\" + QString::number(pngCounter) + " outside_temperature.png","PNG");

    QPixmap pVbat = chartview_vbat -> grab();
    pVbat.save(".\\Images\\" + QString::number(pngCounter) + " battery_voltage.png","PNG");

    pngCounter++;
}

void MainWindow::on_pngbutton2_clicked()
{
    QPixmap pAx = chartview_ax -> grab();
    pAx.save(".\\Images\\" + QString::number(pngCounter2) + " ax.png","PNG");

    QPixmap pAy = chartview_ay -> grab();
    pAy.save(".\\Images\\" + QString::number(pngCounter2) + " ay.png","PNG");

    QPixmap pAz = chartview_az -> grab();
    pAz.save(".\\Images\\" + QString::number(pngCounter2) + " az.png","PNG");

    pngCounter2++;
}

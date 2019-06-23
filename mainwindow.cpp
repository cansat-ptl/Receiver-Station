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

    QMainWindow::showFullScreen();

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

    series_alt = new QLineSeries;
    series_prs = new QLineSeries;
    series_t2 = new QLineSeries;
    series_vbat = new QLineSeries;
    series_ax = new QLineSeries;
    series_ay = new QLineSeries;
    series_az = new QLineSeries;

    chart_alt -> legend() -> hide();
    chart_alt -> addSeries(series_alt);
    chart_alt -> setTitle("Altitude chart");

    axisX_alt = new QValueAxis;
    axisX_alt -> setLabelFormat("%i");
    axisX_alt -> setTitleText("Seconds");
    chart_alt -> addAxis(axisX_alt, Qt::AlignBottom);
    series_alt -> attachAxis(axisX_alt);

    axisY_alt = new QValueAxis;
    axisY_alt -> setLabelFormat("%i");
    axisY_alt -> setTitleText("Meters");
    axisY_alt -> setMax(0);
    axisY_alt -> setMin(0);
    chart_alt -> addAxis(axisY_alt, Qt::AlignLeft);
    series_alt -> attachAxis(axisY_alt);

    chart_prs -> legend() -> hide();
    chart_prs -> addSeries(series_prs);
    chart_prs -> setTitle("Pressure chart");

    axisX_prs = new QValueAxis;
    axisX_prs -> setLabelFormat("%i");
    axisX_prs -> setTitleText("Seconds");
    chart_prs -> addAxis(axisX_prs, Qt::AlignBottom);
    series_prs -> attachAxis(axisX_prs);

    axisY_prs = new QValueAxis;
    axisY_prs -> setLabelFormat("%i");
    axisY_prs -> setTitleText("kPa");
    axisY_prs -> setMax(0);
    axisY_prs -> setMin(0);
    chart_prs -> addAxis(axisY_prs, Qt::AlignLeft);
    series_prs -> attachAxis(axisY_prs);

    chart_t2 -> legend() -> hide();
    chart_t2 -> addSeries(series_t2);
    chart_t2 -> setTitle("Outside temperature chart");

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

    chart_vbat -> legend() -> hide();
    chart_vbat -> addSeries(series_vbat);
    chart_vbat -> setTitle("Battery voltage chart");

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

    chart_ax -> legend() -> hide();
    chart_ax -> addSeries(series_ax);
    chart_ax -> setTitle("Acceleration by X chart");

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

    chart_ay -> legend() -> hide();
    chart_ay -> addSeries(series_ay);
    chart_ay -> setTitle("Acceleration by Y chart");

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

    chart_az -> legend() -> hide();
    chart_az -> addSeries(series_az);
    chart_az -> setTitle("Acceleration by Z chart");

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



    QString portNameR , baudRateR , portNameA, baudRateA;
    QSettings sett("settings.ini", QSettings::IniFormat);
    sett.beginGroup("MAIN");
    portNameR = sett.value("COM_NAME_R", "COM7").toString();
    baudRateR = sett.value("BAUD_RATE_R", "9600").toString();
    latStation = sett.value("STATION_LATITUDE", "62.03389").toDouble();
    lonStation = sett.value("STATION_LONGITUDE", "129.73306").toDouble();
    altStation = sett.value("STATION_ALTITUDE","85").toDouble();
    portNameA = sett.value("COM_NAME_A", "COM3").toString();
    baudRateA = sett.value("BAUD_RATE_A", "9600").toString();
    sett.endGroup();

    receiver = new QSerialPort(this);
    serialBuffer = "";
    parsed_data = "";

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

    QFile dataOrient(".\\CSVs\\data_orient.csv");
    dataOrient.open(QFile::WriteOnly|QFile::Append);
    QTextStream orientStream(&dataOrient);
    orientStream << "N,ET,AX,AY,AZ\n";
    dataOrient.close();

    QFile dataGPS(".\\CSVs\\data_gps.csv");
    dataGPS.open(QFile::WriteOnly|QFile::Append);
    QTextStream gpsStream(&dataGPS);
    gpsStream << "N,ET,LAT,LON,SAT,ALT\n";
    dataGPS.close();

    QFile dataMain(".\\CSVs\\data_main.csv");
    dataMain.open(QFile::WriteOnly|QFile::Append);
    QTextStream mainStream(&dataMain);
    mainStream << "N,ET,VBAT,ALT,PRS,T1,T2\n";
    dataMain.close();

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
        MainWindow::updateData(parsed_data);
    }
}

void MainWindow::writeToTerminal(QString angles)
{
    angles.append( "\r");
    QByteArray ba = angles.toLatin1();
    antenna->write(ba);
}

void MainWindow::updateData(QString s)
{
    int l = s.length();
    QFile dataPackets(".\\CSVs\\data_packets.txt");
    dataPackets.open(QFile::WriteOnly|QFile::Append);
    QTextStream packetStream(&dataPackets);
    packetStream << s << "\n";
    dataPackets.close();
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
        int n = 0, et = 0, rawAx = 0, rawAy = 0, rawAz = 0, i = 0, pitchRaw = 0, yawRaw = 0, rollRaw = 0;
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
            if (s[i] == '=')
            {
                if (s[i - 1] == 'H')
                {
                    if (s[i - 2] == 'C')
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
                        pitchRaw = temp.toInt();
                        temp = "";
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'W')
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
                        yawRaw = temp.toInt();
                        temp = "";
                    }
                }
            }
            if (s[i] == '=')
            {
                if (s[i - 1] == 'L')
                {
                    if (s[i - 2] == 'L')
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
                        rollRaw = temp.toInt();
                        temp = "";
                    }
                }
            }
        }//

        double ax = (double(rawAx) / 10.0) * 9.81, ay = (double(rawAy) / 10.0) * 9.81, az = (double(rawAz) / 10.0) * 9.81;
        double pitch = double(pitchRaw) /10.0, yaw = double(yawRaw) / 10.0, roll = double(rollRaw) / 10.0;

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

        QString packet;
        QFile dataPYR(".\\animation\\data.js");
        dataPYR.open(QFile::WriteOnly|QFile::Append);
        QTextStream pyrStream(&dataPYR);
        if(!first)
        {
        qint64 size = dataPYR.size();
        dataPYR.resize(size - 3);
        packet = ",\n{TIME:" + QString::number(et) + ", ALT:" + QString::number(altBar) + ", ACL_X:" + QString::number(ax) + ", ACL_Y:" + QString::number(ay) + ", ACL_Z:" + QString::number(az) + ", PITCH:" + QString::number(pitch) + ", ROLL:" + QString::number(roll) + ", HEAD:" + QString::number(yaw) + "}\n);";
        }
        if(first)
        {
        pyrStream << "var data = new Array(\n";
        packet = "{TIME:" + QString::number(et) + ", ALT:" + QString::number(altBar) + ", ACL_X:" + QString::number(ax) + ", ACL_Y:" + QString::number(ay) + ", ACL_Z:" + QString::number(az) + ", PITCH:" + QString::number(pitch) + ", ROLL:" + QString::number(roll) + ", HEAD:" + QString::number(yaw) + "}\n);";
        first = false;
        }
        pyrStream << packet;
        dataPYR.close();

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
            if (!damaged[0] && !damaged[1] && !damaged[2] && !damaged[3] && !damaged[4] && !damaged[5])
            {
                ui -> statusBar -> showMessage("Пакет GPS №" + QString::number(n) + " получен удачно.");
                ui -> gps_terminal -> append(s.trimmed() + "\t OK");
            }
            else
            {
                ui -> statusBar -> showMessage("Последний пакет принят с повреждениями, данные могут быть неточны");
                ui -> gps_terminal -> append(s.trimmed() + "\t OK");
            }

            altBar = alt;

            QFile dataGPS(".\\CSVs\\data_gps.csv");
            dataGPS.open(QFile::WriteOnly|QFile::Append);
            QTextStream gpsStream(&dataGPS);
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

            double r1 = 6371200.0 + altStation;
            double x1 = r1 * cos(latStation) * cos(lonStation);
            double y1 = r1 * cos(latStation) * sin(lonStation);
            double z1 = r1 * sin(latStation);

            double r2 = 6371200.0 + alt;
            double x2 = r2 * cos(lat) * cos(lon);
            double y2 = r2 * cos(lat) * sin(lon);
            double z2 = r2 * sin(lat);

            double a = abs(y2 - y1);
            double b = abs(x2 - x1);
            double c = hypot(a, b) / 100.0;
            double d = hypot(c, alt - altStation);

            double alpha = atan2(x2 - x1, y2 - y1) * 180 / M_PI;
            double beta = atan((alt - altStation) / c) * 180 / M_PI;


            if (alpha < 0)
            {
                alpha += 180;
                beta = 180 - beta;
                ui -> flipped -> setText("YES");
            }
            else
            {
                ui -> flipped -> setText("NO");
            }
            if (beta < 0)
            {
                beta = 0;
            }

            ui -> xyz_station -> setText(QString::number(int(x1)) + " " + QString::number(int(y1)) + " " + QString::number(int(z1)));
            ui -> xyz_sat -> setText(QString::number(int(x2)) + " " + QString::number(int(y2)) + " " + QString::number(int(z2)));
            ui -> dist -> setText(QString::number(d));

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
        double vbat = double(vbatRaw) / 10.0;
        int prs = prsRaw / 10000;
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

        altBar = alt;
        QFile dataMain(".\\CSVs\\data_main.csv");
        dataMain.open(QFile::WriteOnly|QFile::Append);
        QTextStream mainStream(&dataMain);
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

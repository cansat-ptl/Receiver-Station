#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QtCharts>

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
     //chart_xyz = new QChart;

     // Creating QLineSeries containers for charts
     series_alt = new QLineSeries;
     series_prs = new QLineSeries;
     series_t2 = new QLineSeries;
     series_vbat = new QLineSeries;
     series_ax = new QLineSeries;
     series_ay = new QLineSeries;
     series_az = new QLineSeries;

     // Conencting an QChart objects with QWidget objects in UI
     ui -> altchart -> setChart(chart_alt);
     ui -> prschart -> setChart(chart_prs);
     ui -> t2chart -> setChart(chart_t2);
     ui -> vbatchart -> setChart(chart_vbat);
     ui -> axchart -> setChart(chart_ax);
     ui -> aychart -> setChart(chart_ay);
     ui -> azchart -> setChart(chart_az);
    // ui -> xyzchart -> setChart(chart_xyz);

     // Altitude chart setting
     chart_alt -> legend() -> hide(); // hide legend
     chart_alt -> addSeries(series_alt); // connecting with QLineSeries
     chart_alt -> setTitle("Altitude chart"); // title of chart
     // Altitude chart axis setting
     axisX_alt = new QValueAxis; // creating a QValueAxis class for X axis of altitude chart
     axisX_alt -> setLabelFormat("%i"); // setting a value axis label format
     axisX_alt -> setTitleText("Seconds"); // title of value axis
     axisX_alt -> setMin(0); // minimal value of X axis (estimated time)
     axisX_alt -> setMax(1); // first maximal value of X axis, will be changed (check updateData() func.)
     chart_alt -> addAxis(axisX_alt, Qt::AlignBottom); // connecting X value axis with altitude chart
     series_alt -> attachAxis(axisX_alt); // connecting QLineSeries container with X value axis

     axisY_alt = new QValueAxis; // creating a QValueAxis class for Y axis of altitude chart
     axisY_alt -> setLabelFormat("%i"); // setting a value axis label format
     axisY_alt -> setTitleText("Meters"); // title of value axis
     axisY_alt -> setMax(1000); // maximal value of altitude
     axisY_alt -> setMin(0); // minimal value of altitude (literally ground)
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
     axisX_prs -> setMin(0); // minimal value of X axis (estimated time)
     axisX_prs -> setMax(1); // first maximal value of X axis, will be changed (check updateData() func.)
     chart_prs -> addAxis(axisX_prs, Qt::AlignBottom); // connecting X value axis with pressure chart
     series_prs -> attachAxis(axisX_prs); // connecting QLineSeries container with X value axis

     axisY_prs = new QValueAxis; // creating a QValueAxis class for Y axis of pressure chart
     axisY_prs -> setLabelFormat("%i"); // setting a value axis label format
     axisY_prs -> setTitleText("Pascals"); // title of value axis
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
     axisX_t2 -> setMin(0);
     axisX_t2 -> setMax(0);
     axisX_t2 -> setTitleText("Seconds");
     chart_t2 -> addAxis(axisX_t2, Qt::AlignBottom);
     series_t2 -> attachAxis(axisX_t2);

     axisY_t2 = new QValueAxis;
     axisY_t2 -> setLabelFormat("%i");
     axisY_t2 -> setTitleText("Celcium");
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
     axisY_vbat -> setLabelFormat("%f");
     axisY_vbat -> setTitleText("Volts");
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
     axisY_ax -> setLabelFormat("%f");
     axisY_ax -> setTitleText("g");
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
     axisY_ay -> setLabelFormat("%f");
     axisY_ay -> setTitleText("g");
     axisY_ay -> setMax(16);
     axisY_ay -> setMin(-16);
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
     axisY_az -> setLabelFormat("%f");
     axisY_az -> setTitleText("g");
     axisY_az -> setMax(16);
     axisY_az -> setMin(-16);
     chart_az -> addAxis(axisY_az, Qt::AlignLeft);
     series_az -> attachAxis(axisY_az);

   /*  // XYZ chart setting
     chart_xyz -> legend() -> show();
     chart_xyz -> addSeries(series_ax);
     chart_xyz -> addSeries(series_ay);
     chart_xyz -> addSeries(series_az);
     chart_xyz -> setTitle("Acceleration by X,Y,Z chart");
     // XYZ chart value axes setting
     axisX_xyz = new QValueAxis;
     axisX_xyz -> setLabelFormat("%i");
     axisX_xyz -> setTickCount(1);
     axisX_xyz -> setTitleText("Seconds");
     chart_xyz -> addAxis(axisX_xyz, Qt::AlignBottom);
     series_ax -> attachAxis(axisX_xyz);
     series_ay -> attachAxis(axisX_xyz);
     series_az -> attachAxis(axisX_xyz);

     axisY_xyz = new QValueAxis;
     axisY_xyz -> setLabelFormat("%f");
     axisY_xyz -> setTitleText("g");
     axisY_xyz -> setMax(16);
     axisY_xyz -> setMin(-16);
     chart_xyz -> addAxis(axisY_xyz, Qt::AlignLeft);
     series_ax -> attachAxis(axisY_xyz);
     series_ay -> attachAxis(axisY_xyz);
     series_az -> attachAxis(axisY_xyz);*/

     // COM port settings
     QString portName = "";
     portName = QInputDialog::getText(this, "Please write COM port name", "COM port name:", QLineEdit::Normal);
     receiver = new QSerialPort(this); // creating a object of receiver using as radio input
     serialBuffer = ""; // buffer for data packets
     parsed_data = ""; // handled version of data packets

     receiver -> setPortName(portName);
     receiver -> open(QSerialPort::ReadOnly);
     receiver -> setBaudRate(QSerialPort::Baud9600);
     receiver -> setDataBits(QSerialPort::Data8);
     receiver -> setFlowControl(QSerialPort::NoFlowControl);
     receiver -> setParity(QSerialPort::NoParity);
     receiver -> setStopBits(QSerialPort::OneStop);

     QObject::connect(receiver, SIGNAL(readyRead()), this, SLOT(readSerial()));

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
        MainWindow::updateData(parsed_data);
    }
}


// Updating data on UI
void MainWindow::updateData(QString s)
{
        int l = s.length();
        bool damaged = false;
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
        if (type == "MAIN")
        {
        int n = 0, et = 0, vbat = 0, alt = 0, prs = 0, t1 = 0, t2 = 0, i = 0;;
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
                        if (i > l)
                        {
                            damaged = true;
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
                            if (i > l)
                            {
                            damaged = true;
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
                                    if (i > l)
                                    {
                                    damaged = true;
                                    break;
                                    }
                                    }
                                vbat = temp.toInt();
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
                                if (i > l)
                                {
                                damaged = true;
                                break;
                                }
                                }
                            alt = temp.toInt();
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
                                if (i > l)
                                {
                                damaged = true;
                                break;
                                }
                                }
                            prs = temp.toInt();
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
                                if (i > l)
                                {
                                damaged = true;
                                break;
                                }
                                }
                            t1 = temp.toInt();
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
                                if (i > l)
                                {
                                damaged = true;
                                break;
                                }
                                }
                            t2 = temp.toInt();
                            temp = "";
                    }
                }
            }
        }
       if (!damaged)
        ui -> statusBar -> showMessage("Главный пакет №" + QString::number(n) + " получен удачно.");
       else
        ui -> statusBar -> showMessage("Последний пакет принят с повреждениями, данные могут быть неточны");
       // Updating labels in right side of UI
       ui -> nvalue -> setText(QString::number(n));
       ui -> etvalue -> setText(QString::number(et));
       ui -> vbatvalue -> setText(QString::number(vbat));
       ui -> altvalue -> setText(QString::number(alt));
       ui -> prsvalue -> setText(QString::number(prs));
       ui -> t1value -> setText(QString::number(t1));
       ui -> t2value -> setText(QString::number(t2));

       // et = estimated time
       // Updating charts and X axis of every chart setting new maximum that is ET

       if (et > etMin1)
       {
           etMin1 = INT_MAX;
           axisX_alt -> setMin(et);
           axisX_prs -> setMin(et);
           axisX_t2 -> setMin(et);
           axisX_vbat -> setMin(et);
           axisX_alt -> setMax(et+1);
           axisX_prs -> setMax(et+1);
           axisX_t2 -> setMax(et+1);
           axisX_vbat -> setMax(et+1);
       }
       else
       {
       axisX_alt -> setMax(et);
       axisX_prs -> setMax(et);
       axisX_t2 -> setMax(et);
       axisX_vbat -> setMax(et);
       }
       if (alt > altMax)
           altMax = alt;
       if (alt < altMin)
           altMin = alt;
       axisY_alt -> setMax(altMax);
       axisY_alt -> setMin(altMin);
       chart_alt -> removeSeries(series_alt);
       series_alt -> append(et , alt);
       chart_alt -> addSeries(series_alt);

       if (prs > prsMax)
           prsMax = prs;
       if (prs < prsMin)
           prsMin = prs;
       axisY_prs -> setMax(prsMax);
       axisY_prs -> setMin(prsMin);
       chart_prs -> removeSeries(series_prs);
       series_prs -> append(et , prs);
       chart_prs -> addSeries(series_prs);

       if (t2 > t2Max)
           t2Max = t2;
       if (t2 < t2Min)
           t2Min = t2;
       axisY_t2 -> setMax(t2Max);
       axisY_t2 -> setMin(t2Min);
       chart_t2 -> removeSeries(series_t2);
       series_t2 -> append(et , t2);
       chart_t2 -> addSeries(series_t2);

       if (vbat > vbatMax)
           vbatMax = vbat;
       if (vbat < vbatMin)
           vbatMin = vbat;
       axisY_vbat -> setMax(vbatMax);
       axisY_vbat -> setMin(vbatMin);
       chart_vbat -> removeSeries(series_vbat);
       series_vbat -> append(et , (double)vbat / 10.0);
       chart_vbat -> addSeries(series_vbat);
       }

       if (type == "ORIENT")
       {
       int n = 0, et = 0, rawAx = 0, rawAy = 0, rawAz = 0, i = 0;
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
                           if (i > l)
                           {
                               damaged = true;
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
                               if (i > l)
                               {
                               damaged = true;
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
                                   if (i > l)
                                   {
                                   damaged = true;
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
                                   if (i > l)
                                   {
                                   damaged = true;
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
                                   if (i > l)
                                   {
                                   damaged = true;
                                   break;
                                   }
                                   }
                               rawAz = temp.toInt();
                               temp = "";
                       }
                   }
               }
           }
       double ax = (double)rawAx / 10.0, ay = (double)rawAy / 10.0, az = (double)rawAz / 10.0;
       if (!damaged)
        ui -> statusBar -> showMessage("Пакет ориентации №" + QString::number(n) + " получен удачно");
       else
        ui -> statusBar -> showMessage("Последний пакет принят с повреждениями, данные могут быть неточны");

       ui -> orient_nvalue -> setText(QString::number(n));
       ui -> orient_etvalue -> setText(QString::number(et));
       ui -> orient_axvalue -> setText(QString::number(ax));
       ui -> orient_ayvalue -> setText(QString::number(ay));
       ui -> orient_azvalue -> setText(QString::number(az));
       if (et > etMin2)
       {
           etMin2 = INT_MAX;
           axisX_ax -> setMin(et);
           axisX_ay -> setMin(et);
           axisX_az -> setMin(et);
       }
       axisX_ax -> setMax(et);
       axisX_ay -> setMax(et);
       axisX_az -> setMax(et);
       if (ax > axMax)
           axMax = ax;
       if (ax< axMin)
           axMin = ax;
       axisY_ax -> setMax(axMax);
       axisY_ax -> setMin(axMin);
       chart_ax -> removeSeries(series_ax);
       series_ax -> append(et , ax);
       chart_ax -> addSeries(series_ax);

       if (ay > ayMax)
           ayMax = ay;
       if (ay < ayMin)
           ayMin = ay;
       axisY_ay -> setMax(ayMax);
       axisY_ay -> setMin(ayMin);
       chart_ay -> removeSeries(series_ay);
       series_ay -> append(et , ay);
       chart_ay -> addSeries(series_ay);

       if (az > azMax)
           azMax = az;
       if (az < azMin)
           azMin = az;
       axisY_az -> setMax(azMax);
       axisY_az -> setMin(azMin);
       chart_az -> removeSeries(series_az);
       series_az -> append(et , az);
       chart_az -> addSeries(series_az);
       }
}


void MainWindow::on_pngbutton1_clicked()
{
    QPixmap pAlt = ui -> altchart -> grab();
    pAlt.save(".\\Images\\" + QString::number(pngCounter) + " altitude.png","PNG");

    QPixmap pPrs = ui -> prschart -> grab();
    pPrs.save(".\\Images\\" + QString::number(pngCounter) + " pressure.png","PNG");

    QPixmap pT2 = ui -> t2chart -> grab();
    pT2.save(".\\Images\\" + QString::number(pngCounter) + " outside_temperature.png","PNG");

    QPixmap pVbat = ui -> vbatchart -> grab();
    pVbat.save(".\\Images\\" + QString::number(pngCounter) + " battery_voltage.png","PNG");

    pngCounter++;
}

void MainWindow::on_pngbutton2_clicked()
{
    QPixmap pAx = ui -> axchart -> grab();
    pAx.save(".\\Images\\" + QString::number(pngCounter2) + " ax.png","PNG");

    QPixmap pAy = ui -> aychart -> grab();
    pAy.save(".\\Images\\" + QString::number(pngCounter2) + " ay.png","PNG");

    QPixmap pAz = ui -> azchart -> grab();
    pAz.save(".\\Images\\" + QString::number(pngCounter2) + " az.png","PNG");

    pngCounter2++;
}

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
     ui->setupUi(this);
     // Creating charts for altitude, pressure, outside temperature (t2) and battery voltage (vbat)
     chart_alt = new QChart;
     chart_prs = new QChart;
     chart_t2 = new QChart;
     chart_vbat = new QChart;

     // Creating QLineSeries containers for charts
     series_alt = new QLineSeries;
     series_prs = new QLineSeries;
     series_t2 = new QLineSeries;
     series_vbat = new QLineSeries;

     // Conencting an QChart objects with QWidget objects in UI
     ui->altchart->setChart(chart_alt);
     ui->prschart->setChart(chart_prs);
     ui->t2chart->setChart(chart_t2);
     ui->vbatchart->setChart(chart_vbat);

     // Altitude chart setting
     chart_alt->legend()->hide(); // hide legend
     chart_alt->addSeries(series_alt); // connecting with QLineSeries
     chart_alt->setTitle("Altitude chart"); // title of chart
     // Altitude chart axis setting
     axisX_alt = new QValueAxis; // creating a QValueAxis class for X axis of altitude chart
     axisX_alt->setLabelFormat("%i"); // setting a value axis label format
     axisX_alt->setTitleText("Seconds"); // title of value axis
     axisX_alt->setMin(0); // minimal value of X axis (estimated time)
     axisX_alt->setMax(1); // first maximal value of X axis, will be changed (check updateData() func.)
     chart_alt->addAxis(axisX_alt, Qt::AlignBottom); // connecting X value axis with altitude chart
     series_alt->attachAxis(axisX_alt); // connecting QLineSeries container with X value axis

     axisY_alt = new QValueAxis; // creating a QValueAxis class for Y axis of altitude chart
     axisY_alt->setLabelFormat("%i"); // setting a value axis label format
     axisY_alt->setTitleText("Altitude"); // title of value axis
     axisY_alt->setMax(1000); // maximal value of altitude
     axisY_alt->setMin(0); // minimal value of altitude (literally ground)
     chart_alt->addAxis(axisY_alt, Qt::AlignLeft); // connecting QLineSeries container with Y value axis
     series_alt->attachAxis(axisY_alt);  // connecting QLineSeries container with Y value axis

     // Pressure chart setting
     chart_prs->legend()->hide(); // hide legend
     chart_prs->addSeries(series_prs); // connecting with QLineSeries
     chart_prs->setTitle("Pressure chart"); // title of chart
     // Pressure chart axis setting
     axisX_prs = new QValueAxis; // creating a QValueAxis class for X axis of pressure chart
     axisX_prs->setLabelFormat("%i"); // setting a value axis label format
     axisX_prs->setTitleText("Seconds"); // title of value axis
     axisX_prs->setMin(0); // minimal value of X axis (estimated time)
     axisX_prs->setMax(1); // first maximal value of X axis, will be changed (check updateData() func.)
     chart_prs->addAxis(axisX_prs, Qt::AlignBottom); // connecting X value axis with pressure chart
     series_prs->attachAxis(axisX_prs); // connecting QLineSeries container with X value axis

     axisY_prs = new QValueAxis; // creating a QValueAxis class for Y axis of pressure chart
     axisY_prs->setLabelFormat("%i"); // setting a value axis label format
     axisY_prs->setTitleText("Pressure"); // title of value axis
     axisY_prs->setMax(120000); // maximal value of pressure
     axisY_prs->setMin(95000); // minimal value of altitude
     chart_prs->addAxis(axisY_prs, Qt::AlignLeft); // connecting QLineSeries container with X value axis
     series_prs->attachAxis(axisY_prs); // connecting QLineSeries container with X value axis

     // Next charts have absolutely same settings
     // Outside temperature chart setting
     chart_t2->legend()->hide();
     chart_t2->addSeries(series_t2);
     chart_t2->setTitle("Outside temperature chart");
     // Outside temperature chart axis setting
     axisX_t2 = new QValueAxis;
     axisX_t2->setLabelFormat("%i");
     axisX_t2->setMin(0);
     axisX_t2->setMax(0);
     axisX_t2->setTitleText("Seconds");
     chart_t2->addAxis(axisX_t2, Qt::AlignBottom);
     series_t2->attachAxis(axisX_t2);

     axisY_t2 = new QValueAxis;
     axisY_t2->setLabelFormat("%i");
     axisY_t2->setTitleText("Outside temperature");
     axisY_t2->setMax(50);
     axisY_t2->setMin(0);
     chart_t2->addAxis(axisY_t2, Qt::AlignLeft);
     series_t2->attachAxis(axisY_t2);

     // Battery voltage chart setting
     chart_vbat->legend()->hide();
     chart_vbat->addSeries(series_vbat);
     chart_vbat->setTitle("Battery voltage chart");
     // Battery voltage chart axis setting
     axisX_vbat = new QValueAxis;
     axisX_vbat->setLabelFormat("%i");
     axisX_vbat->setTickCount(1);
     axisX_vbat->setTitleText("Seconds");
     chart_vbat->addAxis(axisX_vbat, Qt::AlignBottom);
     series_vbat->attachAxis(axisX_vbat);

     axisY_vbat = new QValueAxis;
     axisY_vbat->setLabelFormat("%f");
     axisY_vbat->setTitleText("Battery voltage");
     axisY_vbat->setMax(9);
     axisY_vbat->setMin(6);
     chart_vbat->addAxis(axisY_vbat, Qt::AlignLeft);
     series_vbat->attachAxis(axisY_vbat);




     arduino = new QSerialPort(this); // creating a object of arduino using as radio input
     serialBuffer = ""; // buffer for data packets
     parsed_data = ""; // handled version of data packets
     // Finding an Arduino UNO in all COM ports
     bool arduino_is_available = false;
     QString arduino_uno_port_name;
     foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
         if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
             if((serialPortInfo.productIdentifier() == arduino_uno_product_id)
                     && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)){
                 arduino_is_available = true;
                 arduino_uno_port_name = serialPortInfo.portName();
             }
         }
     }

     // COM port settings
     if(arduino_is_available){
         qDebug() << "Found the arduino port...\n";
         arduino->setPortName(arduino_uno_port_name);
         arduino->open(QSerialPort::ReadOnly);
         arduino->setBaudRate(QSerialPort::Baud9600);
         arduino->setDataBits(QSerialPort::Data8);
         arduino->setFlowControl(QSerialPort::NoFlowControl);
         arduino->setParity(QSerialPort::NoParity);
         arduino->setStopBits(QSerialPort::OneStop);
         QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
     }else{
         qDebug() << "Couldn't find the correct port for the arduino.\n";
         QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
     }

}

MainWindow::~MainWindow()
{
    if(arduino->isOpen()){
        arduino->close(); //    Close the serial port if it's open.
    }
    delete ui;
}

// Reading data from serial port
void MainWindow::readSerial()
{
    QStringList buffer_split = serialBuffer.split("\n");
    if(buffer_split.length() < 2){
        serialData = arduino->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }else{
        serialBuffer = "";
        parsed_data = buffer_split[0];
        MainWindow::updateData(parsed_data);
    }
}


// Updating data on UI
void MainWindow::updateData(QString data)
{
   // Regexp expression to find all numbers in data packet
   QRegExp rx("(\\d+)");
   QStringList list;
   int pos = 0;
   while ((pos = rx.indexIn(data, pos)) != -1) {
       list << rx.cap(1);
       pos += rx.matchedLength();
   }
   // Unscathed data packet have 10 digits
   if (list.count() == 10)
   {
       ui->statusBar->showMessage("Пакет №" + list[1] + " получен удачно.");
       // Updating labels in right side of UI
       ui->nvalue->setText(list[1]);
       ui->etvalue->setText(list[2]);
       ui->vbatvalue->setText(list[3]);
       ui->altvalue->setText(list[4]);
       ui->prsvalue->setText(list[5]);
       ui->t1value->setText(list[7]);
       ui->t2value->setText(list[9]);

       // et = estimated time
       // Updating charts and X axis of every chart setting new maximum that is ET
       int et = list[2].toInt();
       axisX_alt->setMax(et);
       axisX_prs->setMax(et);
       axisX_t2->setMax(et);
       axisX_vbat->setMax(et);

       chart_alt->removeSeries(series_alt);
       series_alt->append(et,list[4].toInt());
       chart_alt->addSeries(series_alt);

       chart_prs->removeSeries(series_prs);
       series_prs->append(et,list[5].toInt());
       chart_prs->addSeries(series_prs);

       chart_t2->removeSeries(series_t2);
       series_t2->append(et,list[9].toInt());
       chart_t2->addSeries(series_t2);

       chart_vbat->removeSeries(series_vbat);
       series_vbat->append(et,(list[3].toDouble()/10.0));
       chart_vbat->addSeries(series_vbat);
   }
   else {
       ui->statusBar->showMessage("Пакет поврежден. Ожидание следующего пакета.");
   }
   qDebug() << data;
}


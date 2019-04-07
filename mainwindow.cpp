#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QtCharts>

QLineSeries *series_alt = new QLineSeries();
QLineSeries *series_prs = new QLineSeries();
QLineSeries *series_t2 = new QLineSeries();
QLineSeries *series_vbat = new QLineSeries();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
     ui->setupUi(this);
     chart_alt = new QChart;
     chart_prs = new QChart;
     chart_t2 = new QChart;
     chart_vbat = new QChart;
     ui->altchart->setChart(chart_alt);
     ui->prschart->setChart(chart_prs);
     ui->t2chart->setChart(chart_t2);
     ui->vbatchart->setChart(chart_vbat);

     chart_alt->legend()->hide();
     chart_alt->addSeries(series_alt);
     chart_alt->createDefaultAxes();
     chart_alt->setTitle("Altitude chart");

     chart_prs->legend()->hide();
     chart_prs->addSeries(series_prs);
     chart_prs->createDefaultAxes();
     chart_prs->setTitle("Pressure chart");

     chart_t2->legend()->hide();
     chart_t2->addSeries(series_t2);
     chart_t2->createDefaultAxes();
     chart_t2->setTitle("Outside temperature chart");

     chart_vbat->legend()->hide();
     chart_vbat->addSeries(series_vbat);
     chart_vbat->createDefaultAxes();
     chart_vbat->setTitle("Battery voltage chart");

     arduino = new QSerialPort(this);
     serialBuffer = "";
     parsed_data = "";
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



void MainWindow::updateData(QString data)
{
   QRegExp rx("(\\d+)");
   QStringList list;
   int pos = 0;
   while ((pos = rx.indexIn(data, pos)) != -1) {
       list << rx.cap(1);
       pos += rx.matchedLength();
   }
   if (list.count() == 10)
   {
       ui->statusBar->showMessage("Пакет №" + list[1] + " получен удачно.");
       ui->nvalue->setText(list[1]);
       ui->etvalue->setText(list[2]);
       ui->vbatvalue->setText(list[3]);
       ui->altvalue->setText(list[4]);
       ui->prsvalue->setText(list[5]);
       ui->t1value->setText(list[7]);
       ui->t2value->setText(list[9]);

       chart_alt->removeSeries(series_alt);
       series_alt->append(list[2].toInt(),list[4].toInt());
       chart_alt->addSeries(series_alt);

       chart_prs->removeSeries(series_prs);
       series_prs->append(list[2].toInt(),list[5].toInt());
       chart_prs->addSeries(series_prs);

       chart_t2->removeSeries(series_t2);
       series_t2->append(list[2].toInt(),list[9].toInt());
       chart_t2->addSeries(series_t2);

       chart_vbat->removeSeries(series_vbat);
       series_vbat->append(list[2].toInt(),list[3].toInt());
       chart_vbat->addSeries(series_vbat);
   }
   else {
       ui->statusBar->showMessage("Пакет поврежден. Ожидание следующего пакета.");
   }
   qDebug() << data;
}


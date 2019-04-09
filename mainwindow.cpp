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
     chart_ax = new QChart;
     chart_ay = new QChart;
     chart_az = new QChart;

     // Creating QLineSeries containers for charts
     series_alt = new QLineSeries;
     series_prs = new QLineSeries;
     series_t2 = new QLineSeries;
     series_vbat = new QLineSeries;
     series_ax = new QLineSeries;
     series_ay = new QLineSeries;
     series_az = new QLineSeries;

     // Conencting an QChart objects with QWidget objects in UI
     ui->altchart->setChart(chart_alt);
     ui->prschart->setChart(chart_prs);
     ui->t2chart->setChart(chart_t2);
     ui->vbatchart->setChart(chart_vbat);
     ui->axchart->setChart(chart_ax);
     ui->aychart->setChart(chart_ay);
     ui->azchart->setChart(chart_az);

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
     axisY_prs->setTitleText("Pascals"); // title of value axis
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
     axisY_t2->setTitleText("Celcium");
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
     axisY_vbat->setTitleText("Volts");
     axisY_vbat->setMax(9);
     axisY_vbat->setMin(6);
     chart_vbat->addAxis(axisY_vbat, Qt::AlignLeft);
     series_vbat->attachAxis(axisY_vbat);

     //AX chart setting
     chart_ax->legend()->hide();
     chart_ax->addSeries(series_ax);
     chart_ax->setTitle("Acceleration by X chart");
     // AX chart axis setting
     axisX_ax = new QValueAxis;
     axisX_ax->setLabelFormat("%i");
     axisX_ax->setTickCount(1);
     axisX_ax->setTitleText("Seconds");
     chart_ax->addAxis(axisX_ax, Qt::AlignBottom);
     series_ax->attachAxis(axisX_ax);

     axisY_ax = new QValueAxis;
     axisY_ax->setLabelFormat("%i");
     axisY_ax->setTitleText("m/s");
     axisY_ax->setMax(100);
     axisY_ax->setMin(0);
     chart_ax->addAxis(axisY_ax, Qt::AlignLeft);
     series_ax->attachAxis(axisY_ax);

     //AY chart setting
     chart_ay->legend()->hide();
     chart_ay->addSeries(series_ay);
     chart_ay->setTitle("Acceleration by Y chart");
     // AX chart axis setting
     axisX_ay = new QValueAxis;
     axisX_ay->setLabelFormat("%i");
     axisX_ay->setTickCount(1);
     axisX_ay->setTitleText("Seconds");
     chart_ay->addAxis(axisX_ay, Qt::AlignBottom);
     series_ay->attachAxis(axisX_ay);

     axisY_ay = new QValueAxis;
     axisY_ay->setLabelFormat("%i");
     axisY_ay->setTitleText("m/s");
     axisY_ay->setMax(100);
     axisY_ay->setMin(0);
     chart_ay->addAxis(axisY_ay, Qt::AlignLeft);
     series_ay->attachAxis(axisY_ay);

     //AZ chart setting
     chart_az->legend()->hide();
     chart_az->addSeries(series_az);
     chart_az->setTitle("Acceleration by Z chart");
     // AZ chart axis setting
     axisX_az = new QValueAxis;
     axisX_az->setLabelFormat("%i");
     axisX_az->setTickCount(1);
     axisX_az->setTitleText("Seconds");
     chart_az->addAxis(axisX_az, Qt::AlignBottom);
     series_az->attachAxis(axisX_az);

     axisY_az = new QValueAxis;
     axisY_az->setLabelFormat("%i");
     axisY_az->setTitleText("m/s");
     axisY_az->setMax(100);
     axisY_az->setMin(0);
     chart_az->addAxis(axisY_az, Qt::AlignLeft);
     series_az->attachAxis(axisY_az);

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
       ui->statusBar->showMessage("Главный пакет №" + list[1] + " получен удачно.");
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
   else if (list.count() == 6 && list[1].toInt() % 2 != 0)
   {
       ui->statusBar->showMessage("Пакет ориентации №" + list[1] + " получен удачно");

       ui->orient_nvalue->setText(list[1]);
       ui->orient_etvalue->setText(list[2]);
       ui->orient_axvalue->setText(list[3]);
       ui->orient_ayvalue->setText(list[4]);
       ui->orient_azvalue->setText(list[5]);
       int et = list[2].toInt();

       axisX_ax->setMax(et);
       axisX_ay->setMax(et);
       axisX_az->setMax(et);

       chart_ax->removeSeries(series_ax);
       series_ax->append(et,list[3].toInt());
       chart_ax->addSeries(series_ax);

       chart_ay->removeSeries(series_ay);
       series_ay->append(et,list[4].toInt());
       chart_ay->addSeries(series_ay);

       chart_az->removeSeries(series_az);
       series_az->append(et,list[5].toInt());
       chart_az->addSeries(series_az);
   }
   else
   {
       ui->statusBar->showMessage("Пакет поврежден. Ожидание следующего пакета.");
   }
   qDebug() << data;
}


void MainWindow::on_pngbutton1_clicked()
{
    QPixmap pAlt = ui->altchart->grab();
    pAlt.save("# " + QString::number(pngCounter) + " altitude.png","PNG");

    QPixmap pPrs = ui->prschart->grab();
    pPrs.save("# " + QString::number(pngCounter) + " pressure.png","PNG");

    QPixmap pT2 = ui->t2chart->grab();
    pT2.save("# " + QString::number(pngCounter) + " outside_temperature.png","PNG");

    QPixmap pVbat = ui->vbatchart->grab();
    pVbat.save("# " + QString::number(pngCounter) + " battery_voltage.png","PNG");

    pngCounter++;
}

void MainWindow::on_pngbutton2_clicked()
{
    QPixmap pAx = ui->axchart->grab();
    pAx.save("# " + QString::number(pngCounter2) + " ax.png","PNG");

    QPixmap pAy = ui->aychart->grab();
    pAy.save("# " + QString::number(pngCounter2) + " ay.png","PNG");

    QPixmap pAz = ui->azchart->grab();
    pAz.save("# " + QString::number(pngCounter2) + " az.png","PNG");

    pngCounter2++;
}

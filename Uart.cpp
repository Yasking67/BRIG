#include "Uart.h"
#include "ui_UART.h"
#include <iostream>
#include <stdio.h>
#include <QDebug>
#include "Converts.h"
#include <ctime>
#include <QTime>
#include <QTimer>
#include <cmath>

//Поле зрения 87 градусов по горизонтали
#define ANGLE_RESOLUTION 0.056

bool UART::flag_auto;
bool UART::flag_manual;
int UART::Neuro_flag;
bool UART::telemetry_flag;
int UART::flag_type;
int UART::send_counter;
int UART::reception_counter;
int UART::flight_indicator;
int UART::pixvalue;
QString UART::RPY_text;
QString UART::GPS_text;
QString UART::file_text;
QFile UART::telemetry;
QTime UART::StartRecordTime;

UART::UART(QWidget *parent) : QWidget(parent),ui(new Ui::UART)
{
    ui->setupUi(this);

    flag_auto = true;
    flag_manual = false;
    Neuro_flag = 0;
    flag_type = 1;
    send_counter = 0;
    reception_counter = 0;
    flight_indicator = 0;
    pixvalue = 1;
    telemetry_flag = true;
    QString filename;
    StartRecordTime = QTime::currentTime();
    StartRecordTime.start();
//    filename.append("D:/telemetry");
//    filename.append(StartRecordTime.toString("hhmmss"));
//    filename.append(".txt");
//    qDebug() <<"Filename: "<<filename;
//    telemetry.setFileName(filename);
//    telemetry.open(QIODevice::WriteOnly);
    cycleTimer = new QTimer();
    cycleTimer->setInterval(500);
    cycleTimer->start();

    serial = new QSerialPort(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    ui->cboxCOM->addItem(info.portName());

    connect(ui->btnCOM, SIGNAL(clicked()), this, SLOT(OpenCloseSerial1()));
    connect(ui->btnCoef, SIGNAL(clicked()), this, SLOT(planecoefficients()));
    connect(serial, SIGNAL(readyRead()), this, SLOT(ReadSdata()));
    connect(ui->radiobtn_auto, SIGNAL(clicked()), this, SLOT(radiobtn_auto_Click()));
    connect(ui->radiobtn_manual, SIGNAL(clicked()), this, SLOT(radiobtn_manual_Click()));
    connect(ui->radiobtn_1mode, SIGNAL(clicked()), this, SLOT(radiobtn_1mode_Click()));
    connect(ui->radiobtn_2mode, SIGNAL(clicked()), this, SLOT(radiobtn_2mode_Click()));
    connect(ui->radiobtn_3mode, SIGNAL(clicked()), this, SLOT(radiobtn_3mode_Click()));
    connect(ui->radiobtn_4mode, SIGNAL(clicked()), this, SLOT(radiobtn_4mode_Click()));
    connect(ui->radiobtn_5mode, SIGNAL(clicked()), this, SLOT(radiobtn_5mode_Click()));
    connect(ui->radiobtn_6mode, SIGNAL(clicked()), this, SLOT(radiobtn_6mode_Click()));
    connect(ui->radiobtn_7mode, SIGNAL(clicked()), this, SLOT(radiobtn_7mode_Click()));
    connect(ui->radiobtn_8mode, SIGNAL(clicked()), this, SLOT(radiobtn_8mode_Click()));
    connect(ui->radiobtn_tracker, SIGNAL(clicked()), this, SLOT(radiobtn_tracker_Click()));
    connect(ui->radiobtn_detector, SIGNAL(clicked()), this, SLOT(radiobtn_detector_Click()));
    connect(this, SIGNAL(onCmdReady1(QByteArray)), this, SLOT(CmdReady1(QByteArray)));
    connect(ui->sliderStep, SIGNAL(valueChanged(int)), this, SLOT(StepChanged(int)));
    connect(ui->btnUp, SIGNAL(clicked()), this, SLOT(ManualUpFly()));
    connect(ui->btnDown, SIGNAL(clicked()), this, SLOT(ManualDownFly()));
    connect(ui->btnLeft, SIGNAL(clicked()), this, SLOT(ManualLeftFly()));
    connect(ui->btnRight, SIGNAL(clicked()), this, SLOT(ManualRightFly()));
    connect(ui->btnDirect, SIGNAL(clicked()), this, SLOT(ManualDirectFly()));
    connect(ui->btnRollMinus, SIGNAL(clicked()), this, SLOT(RollMinus()));
    connect(ui->btnRollPlus, SIGNAL(clicked()), this, SLOT(RollPlus()));
    connect(ui->btnPitchMinus, SIGNAL(clicked()), this, SLOT(PitchMinus()));
    connect(ui->btnPitchPlus, SIGNAL(clicked()), this, SLOT(PitchPlus()));
    connect(ui->btnYawMinus, SIGNAL(clicked()), this, SLOT(YawMinus()));
    connect(ui->btnYawPlus, SIGNAL(clicked()), this, SLOT(YawPlus()));
    connect(ui->btnGasMinus, SIGNAL(clicked()), this, SLOT(GasMinus()));
    connect(ui->btnGasPlus, SIGNAL(clicked()), this, SLOT(GasPlus()));
    connect(this, SIGNAL(ManualFlying(int,int)), this, SLOT(SendManual(int,int)));
//    connect(this, SIGNAL(onWriteInFile()), this, SLOT(WriteInFile()));
    connect(ui->sliderHist, SIGNAL(valueChanged(int)), this, SLOT(HistChanged(int)));
    connect(ui->sliderLmoni, SIGNAL(valueChanged(int)), this, SLOT(LmoniChanged(int)));
    connect(ui->sliderRmoni, SIGNAL(valueChanged(int)), this, SLOT(RmoniChanged(int)));
    connect(ui->sliderLbright, SIGNAL(valueChanged(int)), this, SLOT(LbrightChanged(int)));
    connect(ui->sliderRbright, SIGNAL(valueChanged(int)), this, SLOT(RbrightChanged(int)));
    connect(ui->sliderAvtoexp, SIGNAL(valueChanged(int)), this, SLOT(AvtoexpChanged(int)));
    connect(ui->sliderLvlblack, SIGNAL(valueChanged(int)), this, SLOT(LvlblackChanged(int)));
    connect(ui->sliderEPS, SIGNAL(valueChanged(int)), this, SLOT(EPSChanged(int)));
    connect(ui->sliderThreshhold, SIGNAL(valueChanged(int)), this, SLOT(ThreshholdChanged(int)));
    connect(ui->btnHist, SIGNAL(clicked()), this, SLOT(SendHist()));
    connect(ui->btnLmoni, SIGNAL(clicked()), this, SLOT(SendLmoni()));
    connect(ui->btnRmoni, SIGNAL(clicked()), this, SLOT(SendRmoni()));
    connect(ui->btnLbright, SIGNAL(clicked()), this, SLOT(SendLbright()));
    connect(ui->btnRbright, SIGNAL(clicked()), this, SLOT(SendRbright()));
    connect(ui->btnAvtoexp, SIGNAL(clicked()), this, SLOT(SendAvtoexp()));
    connect(ui->btnLvlblack, SIGNAL(clicked()), this, SLOT(SendLvlblack()));
    connect(ui->btnEPS, SIGNAL(clicked()), this, SLOT(SendEPS()));
    connect(ui->btnThreshhold, SIGNAL(clicked()), this, SLOT(SendThreshhold()));
    connect(ui->btnSaveflash, SIGNAL(clicked()), this, SLOT(SendFlash()));
    connect(ui->radiobtn_usb, SIGNAL(clicked()), this, SLOT(radiobtn_USB_Click()));
    connect(ui->radiobtn_imperx, SIGNAL(clicked()), this, SLOT(radiobtn_imperx_Click()));
    connect(ui->radiobtn_testvideo, SIGNAL(clicked()), this, SLOT(radiobtn_testvideo_Click()));
    connect(ui->radiobtn_raw, SIGNAL(clicked()), this, SLOT(radiobtn_raw_Click()));
    connect(ui->radiobtn_test, SIGNAL(clicked()), this, SLOT(radiobtn_test_Click()));
    connect(ui->radiobtn_obr, SIGNAL(clicked()), this, SLOT(radiobtn_obr_Click()));
    connect(this, SIGNAL(onSetSending()), this, SLOT(SetSending()));
    connect(cycleTimer, SIGNAL(timeout()), this, SLOT(cycleSending()));
    connect(cycleTimer, SIGNAL(timeout()), this, SLOT(WriteInFile()));
    connect(this, SIGNAL(onCycleSending()), this, SLOT(radiobtn_testvideo_Click()));
    connect(ui->btnreset, SIGNAL(clicked()), this, SLOT(ResetCounter()));
    connect(ui->btnOnCamera, SIGNAL(clicked()), this, SLOT(SendCenter()));
    connect(this, SIGNAL(onCameraCenter(int,int)), this, SLOT(SendOnCamera(int,int)));
    connect(ui->btn_calibgyro, SIGNAL(clicked()), this, SLOT(CalibGyro()));
    connect(ui->btn_calibacc, SIGNAL(clicked()), this, SLOT(CalibACC()));
    connect(ui->radiobtn_motoron, SIGNAL(clicked()), this, SLOT(radiobtn_motoron_click()));
    connect(ui->radiobtn_motoroff, SIGNAL(clicked()), this, SLOT(radiobtn_motoroff_click()));
    connect(ui->radiobtn_detectON, SIGNAL(clicked()), this, SLOT(radiobtn_detectON_click()));
    connect(ui->radiobtn_detectOFF, SIGNAL(clicked()), this, SLOT(radiobtn_detectOFF_click()));
    connect(this, SIGNAL(onZero(int,int)), this, SLOT(Zero(int,int)));
    connect(this, SIGNAL(onSendFromDetector(int,int)), this, SLOT(Send(int,int)));
    connect(ui->btnpixhawk, SIGNAL(clicked()), this, SLOT(SendPixhawk()));
    connect(ui->btnpixalt, SIGNAL(clicked()), this, SLOT(SendAltitude()));
    connect(ui->radiobtn_vmin, SIGNAL(clicked()), this, SLOT(radiobtn_vmin_click()));
    connect(ui->radiobtn_vmax, SIGNAL(clicked()), this, SLOT(radiobtn_vmax_click()));
    connect(ui->radiobtn_vcruise, SIGNAL(clicked()), this, SLOT(radiobtn_vcruise_click()));
    connect(ui->radiobtn_throttle, SIGNAL(clicked()), this, SLOT(radiobtn_throttle_click()));
    connect(ui->cbox_Neuro, SIGNAL(clicked()), this, SLOT(CheckBox_Neuro()));
    connect(ui->btnSON, SIGNAL(clicked()), this, SLOT(SendFlightModeSON()));
    connect(ui->btnNPU, SIGNAL(clicked()), this, SLOT(SendFlightModeNPU()));
}

UART::~UART()
{
    delete ui;
}

//Открытие/Закратие UART порта
void UART::OpenCloseSerial1()
{
    if (serial->isOpen())
    {
        serial->close();
        ui->btnCOM->setText("Open Port");
    }
    else
    {

        ui->btnCOM->setText("Close Port");
        serial->setBaudRate(QSerialPort::Baud57600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            if (info.portName() == ui->cboxCOM->currentText())
            {
                serial->setPort(info);
              //  qDebug() <<"SetPort: "<<info.portName();
            }

        }
        serial->open(QSerialPort::ReadWrite);
    }
}

//Отправить коэффициенты на самолёт
void UART::planecoefficients()
{
    QByteArray Data;
    float pitch;
    float roll;
    float yaw;
    float gas;
    float gasscale;
    QString spitch;
    QString sroll;
    QString syaw;
    QString sgas;
    QString sgasscale;
    Data.resize(8);

    spitch = ui->ledit_pitch->text();
    pitch = spitch.toFloat();
    sroll = ui->ledit_roll->text();
    roll = sroll.toFloat();
    syaw = ui->ledit_yaw->text();
    yaw = syaw.toFloat();
    sgas = ui->ledit_gas->text();
    gas = sgas.toFloat();
    sgasscale = ui->ledit_gasscale->text();
    gasscale = sgasscale.toFloat();

    pitch = pitch * 100;
    roll = roll * 100;
    yaw = yaw * 100;
    gas = gas * 100;

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x01;
    Data[2] = roll;
    Data[3] = pitch;
    Data[4] = yaw;
    Data[5] = gas;
    Data[6] = gasscale;
    Data[7] = flag_type;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[8] = ccrc;
    Data[9] = ccrc >> 8;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
    ui->btnCoef->setStyleSheet("background-color: #ff0000");
}

//Отправить углы в авто режиме
void UART::Send(int x, int y)
{
    if (flag_auto)
    {
        QByteArray Data;
        QString text;
        uint16_t ccrc = 0;
        Data.resize(11);
        float angle_x = x * ANGLE_RESOLUTION;
        float angle_y = y * ANGLE_RESOLUTION;
        double x2, y2;
        uint uangle_x = Converts::FloatToUintPack(angle_x);
        uint uangle_y = Converts::FloatToUintPack(angle_y);

        Data[0] = 0xFF;
        Data[1] = uangle_y;
        Data[2] = uangle_y >> 8;
        Data[3] = uangle_y >> 16;
        Data[4] = uangle_y >> 24;
        Data[5] = uangle_x;
        Data[6] = uangle_x >> 8;
        Data[7] = uangle_x >> 16;
        Data[8] = uangle_x >> 24;

        for(int i = 1; i < 9; i++)
        {
            ccrc = ccrc + (uchar)Data[i];
        }

        Data[9] = ccrc;
        Data[10] = ccrc >> 8;

        serial->write(Data);
//        qDebug() << "Data: "<< Data.toHex() << "x, y: " << x << y;
        x2 = double(angle_x);
        y2 = double(angle_y);
//        qDebug() << x << y;
        send_counter = send_counter + 1;
        text.append(" X: ");
        text.append(QString::number(x2,'g',2));
        text.append(" Y: ");
        text.append(QString::number(y2,'g',2));
        text.append("    Sent packages: ");
        text.append(QString::number(send_counter));
        ui->ledit_send->setText(text);
    }
}

//Отправить углы в ручном режиме
void UART::SendManual(int x, int y)
{
    if (flag_manual)
    {
        QByteArray Data;
        QString text;
        uint16_t ccrc = 0;
        Data.resize(11);
        float angle_x = float(x);
        float angle_y = float(y);
        double x2, y2;
        uint uangle_x = Converts::FloatToUintPack(angle_x);
        uint uangle_y = Converts::FloatToUintPack(angle_y);

        Data[0] = 0xFF;
        Data[1] = uangle_y;
        Data[2] = uangle_y >> 8;
        Data[3] = uangle_y >> 16;
        Data[4] = uangle_y >> 24;
        Data[5] = uangle_x;
        Data[6] = uangle_x >> 8;
        Data[7] = uangle_x >> 16;
        Data[8] = uangle_x >> 24;

        for(int i = 1; i < 9; i++)
        {
            ccrc = ccrc + (uchar)Data[i];
        }

        Data[9] = ccrc;
        Data[10] = ccrc >> 8;

        serial->write(Data);
//        qDebug() << "Data: "<< Data.toHex() << "x, y: " << x << y;
        x2 = double(angle_x);
        y2 = double(angle_y);
//        qDebug() << x << y;
        send_counter = send_counter + 1;
        text.append(" X: ");
        text.append(QString::number(x2,'g',2));
        text.append(" Y: ");
        text.append(QString::number(y2,'g',2));
        text.append("    Sent packages: ");
        text.append(QString::number(send_counter));
        ui->ledit_send->setText(text);
    }
}

void UART::SendOnCamera(int x, int y)
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x28;
    Data[4] = y >> 8;
    Data[5] = y;
    Data[6] = x >> 8;
    Data[7] = x;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

//Чтение входного буфера
void UART::ReadSdata()
{
    static QByteArray Resbuf;
    static int s;
    static int flagComp;
    QByteArray buffer;
    qint64 size;

    if (Resbuf.isNull())
    {
        Resbuf.resize(20);
    }
    size = serial->readBufferSize();
    buffer.resize(size);
    buffer = serial->readAll();
//    qDebug() <<"Buffer: "<< buffer.toHex();
//    if (buffer.size() > 100)
//    {
//        buffer.resize(0);
//    }
    for(int i=0; i<buffer.size();++i)
    {
        if (flagComp == 1)
        {
            ++s;
            Resbuf.insert(s, buffer.at(i));
        }
        if((uchar)buffer.at(i) == 0xAA)
        {

            Resbuf.insert(s, buffer.at(i));
            flagComp = 1;

        }
        if ((s == 3) & (Resbuf.at(1) == 0x03))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(4);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 13) & (Resbuf.at(1) == 0x01))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(14);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 13) & (Resbuf.at(1) == 0x02))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(14);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 9) & (Resbuf.at(1) == 0x04))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(10);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 2) & (Resbuf.at(1) == 0x06))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(3);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 2) & (Resbuf.at(1) == 0x10))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(3);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 7) & (Resbuf.at(1) == 0x11))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(8);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 5) & (Resbuf.at(1) == 0x12))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(6);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if ((s == 5) & (Resbuf.at(1) == 0x13))
        {
            s =  0;
            flagComp = 0;
            Resbuf.resize(6);
//            qDebug() <<"ResBuf: "<< Resbuf.toHex();
            emit onCmdReady1(Resbuf);
        }
        if (Resbuf.size() > 100)
        {
            Resbuf.resize(0);
            s =  0;
        }
//        qDebug() <<"ResBuf: "<< Resbuf.toHex();
    }

}

//Обработка данных входного буфера
void UART::CmdReady1(QByteArray buf)
{
//    qDebug() <<"ResBufcmdReady: "<< buf.toHex();
    if (buf.at(1) == 2)
    {
        if (buf.size() == 14)
        {
            uint latu, lonu, altu;
            QByteArray temp;
            temp.resize(4);
            float latf, lonf, altf;
            double lat, lon, alt;

            temp[0] = buf[2];
            temp[1] = buf[3];
            temp[2] = buf[4];
            temp[3] = buf[5];
            memcpy(&latu, temp.constData(), 4);
            temp[0] = buf[6];
            temp[1] = buf[7];
            temp[2] = buf[8];
            temp[3] = buf[9];
            memcpy(&lonu, temp.constData(), 4);
            temp[0] = buf[10];
            temp[1] = buf[11];
            temp[2] = buf[12];
            temp[3] = buf[13];
            memcpy(&altu, temp.constData(), 4);

            latf = Converts::FloatFromUintUnpack(latu);
            lonf = Converts::FloatFromUintUnpack(lonu);
            altf = Converts::FloatFromUintUnpack(altu);

            lat = double(latf);
            lon = double(lonf);
            alt = double(altf);

            GPS_text.clear();
            GPS_text.append(" GPS. Lat: ");
            GPS_text.append(QString::number(lat,'g',5));
            GPS_text.append(" Lon: ");
            GPS_text.append(QString::number(lon,'g',5));
            GPS_text.append(" Alt: ");
            GPS_text.append(QString::number(alt,'g',5));
            ui->ledit_gps->setText(GPS_text);
            emit onWriteInFile();
        }
    }
    if (buf.at(1) == 1)
    {
        if (buf.size() == 14)
        {
            uint rollu, pitchu, yawu;
            QByteArray temp;
            temp.resize(4);
            float rollf, pitchf, yawf;
            double roll, pitch, yaw;

            temp[0] = buf[2];
            temp[1] = buf[3];
            temp[2] = buf[4];
            temp[3] = buf[5];
            memcpy(&rollu, temp.constData(), 4);
            temp[0] = buf[6];
            temp[1] = buf[7];
            temp[2] = buf[8];
            temp[3] = buf[9];
            memcpy(&pitchu, temp.constData(), 4);
            temp[0] = buf[10];
            temp[1] = buf[11];
            temp[2] = buf[12];
            temp[3] = buf[13];
            memcpy(&yawu, temp.constData(), 4);

            rollf = Converts::FloatFromUintUnpack(rollu);
            pitchf = Converts::FloatFromUintUnpack(pitchu);
            yawf = Converts::FloatFromUintUnpack(yawu);

            roll = double(rollf / 3.14 * 180);
            pitch = double(pitchf / 3.14 * 180);
            yaw = double(yawf / 3.14 * 180);
            if (yaw < 0)
            {
                yaw = yaw + 360;
            }

            RPY_text.clear();
            RPY_text.append(" Roll: ");
            RPY_text.append(QString::number(roll,'g',2));
            RPY_text.append(" Pitch: ");
            RPY_text.append(QString::number(pitch,'g',2));
            RPY_text.append(" Yaw: ");
            RPY_text.append(QString::number(yaw,'g',2));
            ui->ledit_rpf->setText(RPY_text);
            emit onWriteInFile();
        }
    }
    if (buf.at(1) == 3)
    {
        if (buf.size() == 4)
        {
            QString text;
            time_t now = time(0);
            char* dt = ctime(&now);

            text.append("Heart bit is ok: ");
            text.append(dt);
            ui->ledit_hb->setText(text);
            text.clear();

            int flight_mode;
            flight_mode = buf[2];
            if (flight_mode == 0)
            {
                text.append("Manual");
                ui->ledit_mode_2->setText(text);
            }
            else if (flight_mode == 1)
            {
                text.append("SON");
                ui->ledit_mode_2->setText(text);
                ui->btnSON->setStyleSheet("background-color:#008000");
            }
            else if (flight_mode == 2)
            {
                text.append("NPU");
                ui->ledit_mode_2->setText(text);
                ui->btnNPU->setStyleSheet("background-color:#008000");
            }
            int battery_charge;
            battery_charge = int(buf[3]);
            uchar batterypersent = uchar(buf[3]);
//            qDebug() <<"battery_charge: "<< buf.toHex();
            text.clear();
            text.append(QString::number(batterypersent,'g',3));
            ui->ledit_battery->setText(text);
            if (battery_charge > 20)
            {ui->label_battery->setStyleSheet("background-color:#008000");}
            else {ui->label_battery->setStyleSheet("background-color:#ff0000");};

        }
    }
    if (buf.at(1) == 4)
    {
        if (buf.size() == 10)
        {
            uint xu,yu;
            QString text;
            QByteArray temp;
            float xf, yf;

            temp.resize(4);
            temp[0] = buf[2];
            temp[1] = buf[3];
            temp[2] = buf[4];
            temp[3] = buf[5];
            memcpy(&xu, temp.constData(), 4);

//            qDebug() <<"xu: "<< xu;
            temp[0] = buf[6];
            temp[1] = buf[7];
            temp[2] = buf[8];
            temp[3] = buf[9];
            memcpy(&yu, temp.constData(), 4);

            xf = Converts::FloatFromUintUnpack(xu);
            yf = Converts::FloatFromUintUnpack(yu);

            send_counter = send_counter + 1;
            text.append("X: ");
            text.append(QString::number(xf,'g',2));
            text.append(" Y: ");
            text.append(QString::number(yf,'g',2));
            text.append("    Packages accepted: ");
            text.append(QString::number(send_counter));
            ui->ledit_read->setText(text);
        }
    }
    if (buf.at(1) == 6)
    {
        if (buf.size() == 3)
        {
            QString text;
            int mode;
            mode = buf[2];
            if (mode == 17)
            {
                text.append("Mode 1");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
            else if (mode == 18)
            {
                text.append("Mode 2");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
            else if (mode == 19)
            {
                text.append("Mode 3");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
            else if (mode == 20)
            {
                text.append("Mode 4");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
            else if (mode == 21)
            {
                text.append("Mode 5");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
            else if (mode == 22)
            {
                text.append("Mode 6");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
            else if (mode == 23)
            {
                text.append("Mode 7");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
            else if (mode == 24)
            {
                text.append("Mode 8");
                ui->ledit_mode->setText(text);
                ui->btnCoef->setStyleSheet("background-color:#008000");
            }
        }
    }
    if (buf.at(1) == 0x10)
    {
        if (buf.size() == 3)
        {
            QString text;
            int mode;
            mode = buf[2];
            if (mode == 1)
            {
                text.append("Ok");
                ui->ledit_sending->setText(text);
            }
            else if (mode == 0)
            {
                text.append("Failed");
                ui->ledit_sending->setText(text);
            }
        }
    }
    if (buf.at(1) == 0x11)
    {
        if (buf.size() == 8)
        {
            QByteArray Data;
            Data.resize(2);
            uint x, y;
            uint16_t ccrc = 0;
            QString text;
//            time_t now = time(0);
//            char* dt = ctime(&now);
//            qDebug() <<"Buf coord: "<< buf.toHex();
            for(int i = 2; i < 6; i++)
            {
                ccrc = ccrc + (uchar)buf[i];
            }
            Data[1] = ccrc >> 8;
            Data[0] = ccrc;
            if (buf[6] == Data[0] & buf[7] == Data[1])
            {
                x = ((uint)buf[4] & 0xFF) << 8;
                x |= (uint)buf[5] & 0xFF;
                y = ((uint)buf[2] & 0xFF) << 8;
                y |= (uint)buf[3] & 0xFF;
//                x = (uint)(buf[4] << 8);
//                x += buf[5];
//                y = (uint)(buf[2] << 8);
//                y += buf[3];
//                x  = ((buf[4] << 8) + buf[5]);
//                y  = ((buf[2] << 8) + buf[3]);
//                x = round(x);
//                y = round(y);
//                qDebug() <<"XY "<< x << y;
                if (flight_indicator == 1)
                {
                    int pixx = x - 960;
                    int pixy = 540 - y;
                    emit onSendFromDetector(pixx,pixy);
                }
                text.clear();
                text.append("x = ");
                text.append(QString::number(x,'g',3));
                ui->ledit_detectX->setText(text);
                text.clear();
                text.append("y = ");
                text.append(QString::number(y,'g',3));
                ui->ledit_detectY->setText(text);
            }
            else
            {
                text.append("CRC failed");
                ui->ledit_detectX->setText(text);
                ui->ledit_detectY->setText(text);
            }
        }
    }
    if (buf.at(1) == 0x12)
    {
        if (buf.size() == 6)
        {
            uint uvalue;
            QString text;
            QByteArray temp;
            float value;

            temp.resize(4);
            temp[0] = buf[2];
            temp[1] = buf[3];
            temp[2] = buf[4];
            temp[3] = buf[5];
            memcpy(&uvalue, temp.constData(), 4);
            value = Converts::FloatFromUintUnpack(uvalue);
            text.append(QString::number(value,'g',5));
            ui->ledit_pixread->setText(text);
            ui->btnpixhawk->setStyleSheet("background-color:#008000");
        }
    }
    if (buf.at(1) == 0x13)
    {
        if (buf.size() == 6)
        {
            uint uvalue;
            QString text;
            QByteArray temp;
            float value;

            temp.resize(4);
            temp[0] = buf[2];
            temp[1] = buf[3];
            temp[2] = buf[4];
            temp[3] = buf[5];
            memcpy(&uvalue, temp.constData(), 4);
            value = Converts::FloatFromUintUnpack(uvalue);
            text.append(QString::number(value,'g',5));
            ui->ledit_pixread->setText(text);
            ui->btnpixalt->setStyleSheet("background-color:#008000");
        }
    }

}

void UART::WriteInFile()
{
    if (ui->cbox_telemetry->isChecked())
    {
        time_t now = time(0);
        char* dt = ctime(&now);
        file_text.clear();
        file_text.append(dt);
        file_text.append(RPY_text);
        file_text.append(GPS_text);
        telemetry.write(file_text.toUtf8());
        telemetry.write("\n");
        telemetry.flush();
    }
}

//Ручной полёт вверх
void UART::ManualUpFly()
{
    QString sStep;
    sStep = ui->ledit_step->text();
    int Step = sStep.toInt();
    if (ui->cbox_zero->isChecked())
    {
        emit onZero(-Step,2);
    }
    else if (flag_manual)
    {
        emit ManualFlying(0,Step);
    }
}

//Ручной полёт вниз
void UART::ManualDownFly()
{
    QString sStep;
    sStep = ui->ledit_step->text();
    int Step = sStep.toInt();
    if (ui->cbox_zero->isChecked())
    {
        emit onZero(Step,2);
    }
    else if (flag_manual)
    {
        emit ManualFlying(0,(-Step));
    }
}

//Ручной полёт влево
void UART::ManualLeftFly()
{
    QString sStep;
    sStep = ui->ledit_step->text();
    int Step = sStep.toInt();
    if (ui->cbox_zero->isChecked())
    {
        emit onZero(Step,1);
    }
    else if (flag_manual)
    {
        emit ManualFlying((-Step),0);
    }
}

//Ручной полёт вправо
void UART::ManualRightFly()
{
    QString sStep;
    sStep = ui->ledit_step->text();
    int Step = sStep.toInt();
    if (ui->cbox_zero->isChecked())
    {
        emit onZero(-Step,1);
    }
    else if (flag_manual)
    {
        emit ManualFlying(Step,0);
    }
}

//Ручной полёт прямо
void UART::ManualDirectFly()
{
    if (flag_manual)
    {
        emit ManualFlying(0,0);
    }
}

//Вывод значения Step на треминал
void UART::StepChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->ledit_step->setText(text);
}

//Обработка режимов полёта
void UART::radiobtn_auto_Click()
{
    flag_auto = true;
    flag_manual = false;
}

void UART::radiobtn_manual_Click()
{
    flag_auto = false;
    flag_manual = true;
}

void UART::radiobtn_1mode_Click()
{
    flag_type = 1;
}

void UART::radiobtn_2mode_Click()
{
    flag_type = 2;
}

void UART::radiobtn_3mode_Click()
{
    flag_type = 3;
}

void UART::radiobtn_4mode_Click()
{
    flag_type = 4;
}

void UART::radiobtn_5mode_Click()
{
    flag_type = 5;
}

void UART::radiobtn_6mode_Click()
{
    flag_type = 6;
}

void UART::radiobtn_7mode_Click()
{
    flag_type = 7;
}

void UART::radiobtn_8mode_Click()
{
    flag_type = 8;
}

void UART::radiobtn_tracker_Click()
{
    flight_indicator = 0;
}

void UART::radiobtn_detector_Click()
{
    flight_indicator = 1;
}

void UART::RollMinus()
{
    QString sroll;
    float roll;
    sroll = ui->ledit_roll->text();
    roll = sroll.toFloat();
    roll = roll - 0.1;
    sroll.clear();
    sroll.append(QString::number(roll,'g',2));
    ui->ledit_roll->setText(sroll);
}

void UART::RollPlus()
{
    QString sroll;
    float roll;
    sroll = ui->ledit_roll->text();
    roll = sroll.toFloat();
    roll = roll + 0.1;
    sroll.clear();
    sroll.append(QString::number(roll,'g',2));
    ui->ledit_roll->setText(sroll);
}

void UART::PitchMinus()
{
    QString spitch;
    float pitch;
    spitch = ui->ledit_pitch->text();
    pitch = spitch.toFloat();
    pitch = pitch - 0.1;
    spitch.clear();
    spitch.append(QString::number(pitch,'g',2));
    ui->ledit_pitch->setText(spitch);
}

void UART::PitchPlus()
{
    QString spitch;
    float pitch;
    spitch = ui->ledit_pitch->text();
    pitch = spitch.toFloat();
    pitch = pitch + 0.1;
    spitch.clear();
    spitch.append(QString::number(pitch,'g',2));
    ui->ledit_pitch->setText(spitch);
}

void UART::YawMinus()
{
    QString syaw;
    float yaw;
    syaw = ui->ledit_yaw->text();
    yaw = syaw.toFloat();
    yaw = yaw - 0.1;
    syaw.clear();
    syaw.append(QString::number(yaw,'g',2));
    ui->ledit_yaw->setText(syaw);
}

void UART::YawPlus()
{
    QString syaw;
    float yaw;
    syaw = ui->ledit_yaw->text();
    yaw = syaw.toFloat();
    yaw = yaw + 0.1;
    syaw.clear();
    syaw.append(QString::number(yaw,'g',2));
    ui->ledit_yaw->setText(syaw);
}

void UART::GasMinus()
{
    QString sgas;
    float gas;
    sgas = ui->ledit_gas->text();
    gas = sgas.toFloat();
    gas = gas - 0.1;
    sgas.clear();
    sgas.append(QString::number(gas,'g',2));
    ui->ledit_gas->setText(sgas);
}

void UART::GasPlus()
{
    QString sgas;
    float gas;
    sgas = ui->ledit_gas->text();
    gas = sgas.toFloat();
    gas = gas + 0.1;
    sgas.clear();
    sgas.append(QString::number(gas,'g',2));
    ui->ledit_gas->setText(sgas);
}

void UART::radiobtn_USB_Click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x00;
    Data[3] = 0x01;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x01;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
    emit onSetSending();
}

void UART::radiobtn_imperx_Click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x00;
    Data[3] = 0x01;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x02;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
    emit onSetSending();
}

void UART::radiobtn_testvideo_Click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x00;
    Data[3] = 0x01;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x03;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
    emit onSetSending();
}

void UART::radiobtn_raw_Click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x00;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x01;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
    emit onSetSending();
}

void UART::radiobtn_test_Click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x00;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x06;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
    emit onSetSending();
}

void UART::radiobtn_obr_Click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x00;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x0D;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
    emit onSetSending();
}

void UART::HistChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditHist->setText(text);
}

void UART::LmoniChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditLmoni->setText(text);
}

void UART::RmoniChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditRmoni->setText(text);
}

void UART::LbrightChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditLbright->setText(text);
}

void UART::RbrightChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditRbright->setText(text);
}

void UART::AvtoexpChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditAvtoexp->setText(text);
}

void UART::LvlblackChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditLvlblack->setText(text);
}

void UART::EPSChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditEPS->setText(text);
}

void UART::ThreshholdChanged(int Step)
{
   QString text;
   text.append(QString::number(Step));
   ui->leditThreshhold->setText(text);
}

void UART::SendHist()
{
    QByteArray Data;
    Data.resize(10);
    QString shist;
    float hist;

    shist = ui->leditHist->text();
    hist = shist.toFloat();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x14;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = hist;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendLmoni()
{
    QByteArray Data;
    Data.resize(10);
    QString slmoni;
    float lmoni;

    slmoni = ui->leditLmoni->text();
    lmoni = slmoni.toFloat();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x10;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = lmoni;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendRmoni()
{
    QByteArray Data;
    Data.resize(10);
    QString srmoni;
    float rmoni;

    srmoni = ui->leditRmoni->text();
    rmoni = srmoni.toFloat();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x11;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = rmoni;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendLbright()
{
    QByteArray Data;
    Data.resize(10);
    QString slbr;
    float lbr;

    slbr = ui->leditLbright->text();
    lbr = slbr.toFloat();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x12;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = lbr;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendRbright()
{
    QByteArray Data;
    Data.resize(10);
    QString srbr;
    float rbr;

    srbr = ui->leditRbright->text();
    rbr = srbr.toFloat();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x13;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = rbr;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendFlash()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0xFF;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0xFF;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendAvtoexp()
{
    QByteArray Data;
    Data.resize(10);
    QString savtoexp;
    int avtoexp;

    savtoexp = ui->leditAvtoexp->text();
    avtoexp = savtoexp.toInt();

    uint16_t ccrc = 0;
    uint uavtoexp = Converts::FloatToUintPack(avtoexp);
    qDebug() << "uavtoexp: "<< uavtoexp;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x20;
    Data[3] = 0x00;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = avtoexp >> 8;
    Data[7] = avtoexp;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendLvlblack()
{
    QByteArray Data;
    Data.resize(10);
    QString slvlblack;
    float lvlblack;

    slvlblack = ui->leditLvlblack->text();
    lvlblack = slvlblack.toFloat();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x20;
    Data[3] = 0x10;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = lvlblack;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SetSending()
{
    QString text;
    text.append("Sending...");
    ui->ledit_sending->setText(text);
}

void UART::cycleSending()
{
    if (ui->cbox_cycle->isChecked())
    {
        emit onCycleSending();
    }
}

void UART::ResetCounter()
{
    send_counter = 0;
    reception_counter = 0;
}

void UART::SendEPS()
{
    QByteArray Data;
    Data.resize(10);
    QString seps;
    int eps;

    seps = ui->leditEPS->text();
    eps = seps.toInt();
    eps = eps * 81;

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x20;
    Data[4] = 0x00;
    Data[5] = eps >> 16;
    Data[6] = eps >> 8;
    Data[7] = eps;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendThreshhold()
{
    QByteArray Data;
    Data.resize(10);
    QString sThreshhold;
    int Threshhold;

    sThreshhold = ui->leditThreshhold->text();
    Threshhold = sThreshhold.toInt();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x30;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = Threshhold;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendCenter()
{
    emit onCameraCenter(960,540);
}


void UART::CalibGyro()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x03;
    Data[2] = 0x3E;
    Data[3] = 0x67;
    Data[4] = 0x00;
    Data[5] = 0x67;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
}

void UART::CalibACC()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x03;
    Data[2] = 0x3E;
    Data[3] = 0x41;
    Data[4] = 0x00;
    Data[5] = 0x41;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
}

void UART::radiobtn_motoron_click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x03;
    Data[2] = 0x3E;
    Data[3] = 0x4D;
    Data[4] = 0x00;
    Data[5] = 0x4D;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
}

void UART::radiobtn_motoroff_click()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x03;
    Data[2] = 0x3E;
    Data[3] = 0x6D;
    Data[4] = 0x00;
    Data[5] = 0x6D;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
}

void UART::Zero(int angle, int direction)
{
    QByteArray Data;
    uint16_t ccrc = 0;
    Data.resize(10);
    float anglef = float(angle);
    uint uangle = Converts::FloatToUintPack(anglef);

    Data[0] = 0xAA;
    Data[1] = 0x04;
    Data[2] = uangle;
    Data[3] = uangle >> 8;
    Data[4] = uangle >> 16;
    Data[5] = uangle >> 24;
    Data[6] = direction;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();
//    qDebug() << "Type: "<< flag_type;

    serial->write(Data);
}

void UART::radiobtn_detectON_click()
{
    QByteArray Data;
    Data.resize(10);
    QString sThreshhold;
    int Threshhold;

    sThreshhold = ui->leditThreshhold->text();
    Threshhold = sThreshhold.toInt();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x2F;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x01;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::radiobtn_detectOFF_click()
{
    QByteArray Data;
    Data.resize(10);
    QString sThreshhold;
    int Threshhold;

    sThreshhold = ui->leditThreshhold->text();
    Threshhold = sThreshhold.toInt();

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x02;
    Data[2] = 0x10;
    Data[3] = 0x2F;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
}

void UART::SendPixhawk()
{
    QByteArray Data;
    Data.resize(10);
    QString svalue;
    float value;

    svalue = ui->ledit_pixvalue->text();
    value = svalue.toFloat();
    uint uvalue = Converts::FloatToUintPack(value);


    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x05;
    Data[2] = uvalue;
    Data[3] = uvalue >> 8;
    Data[4] = uvalue >> 16;
    Data[5] = uvalue >> 24;
    Data[6] = pixvalue;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
    ui->btnpixhawk->setStyleSheet("background-color: #ff0000");
}

void UART::radiobtn_vmin_click()
{
    pixvalue = 1;
}
void UART::radiobtn_vmax_click()
{
    pixvalue = 2;
}
void UART::radiobtn_vcruise_click()
{
    pixvalue = 3;
}
void UART::radiobtn_throttle_click()
{
    pixvalue = 4;
}

void UART::SendAltitude()
{
    QByteArray Data;
    Data.resize(10);
    QString svalue;
    float value;

    svalue = ui->ledit_pixalt->text();
    value = svalue.toFloat();
    uint uvalue = Converts::FloatToUintPack(value);


    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x06;
    Data[2] = uvalue;
    Data[3] = uvalue >> 8;
    Data[4] = uvalue >> 16;
    Data[5] = uvalue >> 24;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
    ui->btnpixalt->setStyleSheet("background-color: #ff0000");
}


void UART::CheckBox_Neuro()
{
    if (ui->cbox_Neuro->isChecked())
    {
        Neuro_flag = 1;
    }
    else
    {
        Neuro_flag = 0;
    }
}

void UART::SendFlightModeSON()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x07;
    Data[2] = 0x01;
    Data[3] = 0x00;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
    ui->btnSON->setStyleSheet("background-color: #ff0000");
}

void UART::SendFlightModeNPU()
{
    QByteArray Data;
    Data.resize(10);

    uint16_t ccrc = 0;

    Data[0] = 0xAA;
    Data[1] = 0x07;
    Data[2] = 0x02;
    Data[3] = 0x00;
    Data[4] = 0x00;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x00;

    for(int i = 2; i < 8; i++)
    {
        ccrc = ccrc + (uchar)Data[i];
    }

    Data[9] = ccrc >> 8;
    Data[8] = ccrc;

//    qDebug() << "Data: "<< Data.toHex();

    serial->write(Data);
    emit onSetSending();
    ui->btnNPU->setStyleSheet("background-color: #ff0000");
}


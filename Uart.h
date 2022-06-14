#ifndef UART_H
#define UART_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QFile>

namespace Ui {
class UART;
}


class UART : public QWidget
{
    Q_OBJECT
public:
    explicit UART(QWidget *parent = nullptr);
    ~UART();
    static bool flag_auto;
    static bool flag_manual;
    static bool telemetry_flag;
    static int Neuro_flag;
    static int flag_type;
    static int send_counter;
    static int reception_counter;
    static int flight_indicator;
    static int pixvalue;
    static QString RPY_text;
    static QString GPS_text;
    static QString file_text;
    static QFile telemetry;
    static QTime StartRecordTime;

private:
    Ui::UART *ui;
    QSerialPort *serial;
    QTimer *cycleTimer;

signals:
    void onCmdReady1(QByteArray buf);
    void ManualFlying(int,int);
    void onWriteInFile();
    void onSetSending();
    void onCycleSending();
    void onCameraCenter(int,int);
    void onZero(int,int);
    void onSendFromDetector(int,int);

public slots:
    void OpenCloseSerial1();
    void planecoefficients();
    void Send(int,int);
    void SendManual(int,int);
    void SendOnCamera(int,int);
    void SendCenter();
    void SendPixhawk();
    void ReadSdata();
    void CmdReady1 (QByteArray buf);
    void WriteInFile();
    void radiobtn_auto_Click();
    void radiobtn_manual_Click();
    void radiobtn_1mode_Click();
    void radiobtn_2mode_Click();
    void radiobtn_3mode_Click();
    void radiobtn_4mode_Click();
    void radiobtn_5mode_Click();
    void radiobtn_6mode_Click();
    void radiobtn_7mode_Click();
    void radiobtn_8mode_Click();
    void radiobtn_tracker_Click();
    void radiobtn_detector_Click();
    void StepChanged(int);
    void ManualUpFly();
    void ManualDownFly();
    void ManualRightFly();
    void ManualLeftFly();
    void ManualDirectFly();
    void RollMinus();
    void RollPlus();
    void PitchMinus();
    void PitchPlus();
    void YawMinus();
    void YawPlus();
    void GasMinus();
    void GasPlus();
    void radiobtn_USB_Click();
    void radiobtn_imperx_Click();
    void radiobtn_testvideo_Click();
    void radiobtn_raw_Click();
    void radiobtn_test_Click();
    void radiobtn_obr_Click();
    void HistChanged(int);
    void LmoniChanged(int);
    void RmoniChanged(int);
    void LbrightChanged(int);
    void RbrightChanged(int);
    void AvtoexpChanged(int);
    void LvlblackChanged(int);
    void ThreshholdChanged(int);
    void EPSChanged(int);
    void SendHist();
    void SendLmoni();
    void SendRmoni();
    void SendLbright();
    void SendRbright();
    void SendAvtoexp();
    void SendLvlblack();
    void SendEPS();
    void SendFlash();
    void SendThreshhold();
    void SetSending();
    void cycleSending();
    void ResetCounter();
    void CalibGyro();
    void CalibACC();
    void radiobtn_motoron_click();
    void radiobtn_motoroff_click();
    void radiobtn_detectON_click();
    void radiobtn_detectOFF_click();
    void Zero(int,int);
    void radiobtn_vmin_click();
    void radiobtn_vmax_click();
    void radiobtn_vcruise_click();
    void radiobtn_throttle_click();
    void SendAltitude();
    void CheckBox_Neuro();
    void SendFlightModeSON();
    void SendFlightModeNPU();

};

#endif // UART_H

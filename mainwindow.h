#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMouseEvent>

#include "UDP_Receiver.h"
#include "UDP_Sender.h"
#include "Converts.h"
#include "DataProcessing.h"
#include "ImageProcessing.h"
#include "Form_VideoFrame.h"
#include "Tracker.h"
#include "Uart.h"
#include "RunYOLO.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    UDP_Sender *UDP_Sender_BVR;
    UDP_Receiver *UDP_Receiver_BVR;

    QThread *UDP_SenderThread;
    QThread *UDP_ReceiverThread;
    QThread *ImageProcessingThread;
    QThread *UDP_TrackerThread;

    ImageProcessing *ImageProcessing_BVR;

    Form_VideoFrame *Form_VideoFrame_BVR;

    Tracker *Tracker_BVR;

    UART *UART_BVR;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *pTimer_MW;
    uint TimerCounter, FlagsCounter;
    uint PreviousFrameCounter, CurrentFrameCounter;
    QTime FPS_Time;

    void SendCommand_Click(ushort, uint);
    void SendCCP_Command(ushort);

protected:
//    virtual void mouseMoveEvent(QMouseEvent *);

public slots:
    void WriteDes(QString descr);
    void InitPCAP(void);
    void UpdateFrameCounter(unsigned char fc);
    void PushButton_SendCommand_Click(void);
    void Timer_MW_timeout(void);
    void UART_SendCommand(uchar preamble, uchar size, uchar *data);

    void PrintFlags(unsigned int);
    void CheckBox_FrameRecording(void);

signals:
    void onWriteDescriptor(QString);
    void onInitPCAP(QString);
    void onSendUDP_Packet(QByteArray);
    void onUART_SendCommand(uchar, uchar, QByteArray);
    void onSendUDP_PacketHost(QByteArray, QByteArray, ushort);
    void onSendUDP_PacketHost(QByteArray, uint, ushort);
    void onUART_SendCommand(uchar , uchar , uchar *);
};

#endif // MAINWINDOW_H

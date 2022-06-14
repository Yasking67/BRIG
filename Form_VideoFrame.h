#ifndef FORM_VIDEOFRAME_H
#define FORM_VIDEOFRAME_H

#include <QWidget>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QTime>

#include <iostream>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "UDP_Sender.h"
#include "ImageProcessing.h"
#include "Converts.h"

namespace Ui {
class Form_VideoFrame;
}

class Form_VideoFrame : public QWidget
{
    Q_OBJECT

public:
    explicit Form_VideoFrame(QWidget *parent = 0);
    ~Form_VideoFrame();
    static bool GetVideoViewingFlag(void) {return VideoViewingFlag;}
    static bool GetVideoRecordingFlag(void) {return VideoRecordingFlag;}
    static cv::VideoWriter GetVideoWriter(void) {return writer;}

private:
    Ui::Form_VideoFrame *ui;
    QTimer *pTimer_VF;
    QGraphicsScene *pGraphicsScene_VF;
    virtual bool eventFilter(QObject *, QEvent *);
    void GroupBox_ZoomFactor_Click(uchar);

    static bool VideoViewingFlag, VideoIsOpened, VideoRecordingFlag;
    uint TimerCounter;
    QTime RecordTime, StartRecordTime;

    cv::VideoCapture cap;
    cv::Mat frame;
    static cv::VideoWriter writer;
    std::string VideoRecordFileName;
    double VideoFrameNumber;

    QStringList availableCodecs;
    QVector<int> availableCodecsInt;
    QStringList supportedCodec = {"MJPG", "MRLE", "H264", "MPG4", "XVID", "MP42", "MP43"};
    bool IsCompression;
    int CodecChoiceIndex, VideoViewingFPS;

protected:
//    virtual void mouseMoveEvent(QMouseEvent *);
//    virtual void hoverMoveEvent(QMouseEvent *);

public slots:
    void CheckBox_TestModeOnOff(void);
    void DrawNewFrame(QImage img);
    void DrawNewFrame2(QImage img);
    void WriteNewFrame(uchar *pFrame);
    void WriteNewFrame2(cv::Mat frame);
    void UART_SendCommand_VF(uchar preamble, uchar size, uchar *data);
    void RadioButton_ZoomFactor_x1_Click(void);
    void RadioButton_ZoomFactor_x2_Click(void);
    void RadioButton_ZoomFactor_x4_Click(void);
    void RadioButton_ZoomFactor_x8_Click(void);
    void RadioButton_ZoomFactor_x16_Click(void);
    void Timer_VF_timeout(void);
    void CheckBox_VideoViewing(void);
    void CheckBox_VideoRecording(void);
    void RadioButton_VideoRecordFileName_Manual_Click(void);
    void RadioButton_VideoRecordFileName_Auto_Click(void);
signals:
    void onSendUDP_Packet(QByteArray);
    void onUART_SendCommand_VF(uchar , uchar , uchar *);
    void update_bbox(int,int);
    void send_on_camera(int,int);
};

#endif // FORM_VIDEOFRAME_H

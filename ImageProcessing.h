#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QDebug>

#include "DataProcessing.h"
#include "Form_VideoFrame.h"
#include "Tracker.h"

#define BUFFER_NUMBER 4
#define FRAME_WIDTH 960
#define FRAME_HEIGHT 1080
#define FRAME_TO_IMAGE_SCALE 2
#define IMAGE_WIDTH FRAME_WIDTH
//#define IMAGE_WIDTH FRAME_WIDTH / FRAME_TO_IMAGE_SCALE
#define IMAGE_HEIGHT FRAME_HEIGHT / FRAME_TO_IMAGE_SCALE
#define DOUBLE_FRAME_WIDTH FRAME_WIDTH * 2
#define DOUBLE_FRAME_HEIGHT FRAME_HEIGHT * 2
#define QUADRUPLE_FRAME_WIDTH FRAME_WIDTH * 4
#define FRAME_SIZE FRAME_WIDTH * FRAME_HEIGHT
#define BUFFER_SIZE FRAME_SIZE
#define WRITE_FRAME_SIZE DOUBLE_FRAME_WIDTH * FRAME_HEIGHT
#define WRITE_BUFFER_SIZE WRITE_FRAME_SIZE

class ImageProcessing : public QObject
{
    Q_OBJECT

public:
    explicit ImageProcessing(QObject *parent = 0);
    QTimer *pTimer_IP;
    static ImageProcessing *pCurrentInputImageBuffer;
    static uchar *pImageBuffer[BUFFER_NUMBER];
    static uchar ImageBuffer[][BUFFER_SIZE];
    static uchar WriteFrameBuffer[WRITE_BUFFER_SIZE];
    static bool FrameRecordingFlag, TargetCoordinatesRecordingFlag;
    static QFile VideoFrameData, TargetCoordinatesData;
    static QImage Image_IP;
//    static uint *pStartImageData, *pImageData;
    static QTime ct;
    static cv::Mat frame;

    static void TestImages(void);
    static bool GetModeFlag(void) {return ModeFlag;}
    static void SetModeFlag(bool mf) {ModeFlag = mf;}
    static uchar *GetTestOutputBuffer(void) {return pImageBuffer[TestOutputBufferCounter];}
    uchar *GetInputBuffer(void);
    static uchar *GetInputBufferS(void);
    static uchar *GetOutputBufferS(void) {return pImageBuffer[OutputBufferCounter];}
    uchar GetInputBufferCounter(void) const {return InputBufferCounter;}
    static uint GetFrameCounterS(void) {return FrameCounter;}
    static void SetFrameCounterS(uint fs) {FrameCounter = fs;}
    static uchar GetInputBufferCounterS(void) {return InputBufferCounter;}
    static void SetIsAnyInputBufferFull(void) {IsAnyInputBufferFull = true;}
    static void SetIsInputBufferFull(uchar ibc);
    static uchar *GetWriteFrameBuffer(void) {return &WriteFrameBuffer[0];}
    static uchar GetZoomFactor(void) {return ZoomFactor;}
//    static void SetZoomFactor(uchar zf) {ZoomFactor = zf;}
    static void SetZoomFactor(uchar, ushort, ushort);
    static ushort GetHorizontalZoomCenter(void) {return HorizontalZoomCenter;}
    static void SetHorizontalZoomCenter(ushort hzc) {HorizontalZoomCenter = hzc;}
    static ushort GetVerticalZoomCenter(void) {return VerticalZoomCenter;}
    static void SetVerticalZoomCenter(ushort vzc) {VerticalZoomCenter = vzc;}

private:
    static bool ModeFlag;
    static uchar TestOutputBufferCounter;
    QMutex Mutex_IP;
    static QMutex Mutex_IP_S;
    static bool IsAnyInputBufferFull;
    static bool IsInputBufferFull[BUFFER_NUMBER];
    static uchar InputBufferMask;
    static uchar InputBufferCounter, OutputBufferCounter;
    static uint FrameCounter;
    static uchar ZoomFactor;
    static ushort HorizontalZoomCenter, VerticalZoomCenter;

public slots:
    void Timer_IP_timeout(void);

signals:
     void onShowVideoFrame(QImage);
     void onWriteVideoFrame(uchar *);
     void onTracker(uchar *);
};

#endif // IMAGEPROCESSING_H

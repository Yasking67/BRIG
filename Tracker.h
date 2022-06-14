#ifndef TRACKER_H
#define TRACKER_H

#include <QObject>
#include <ImageProcessing.h>
#include <Form_VideoFrame.h>
#include <Uart.h>
#include <RunYOLO.h>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <QApplication>
#include <ctime>
#include <iostream>


class Tracker : public QObject
{
    Q_OBJECT


public:
    explicit Tracker(QObject *parent = nullptr);
    static QImage imag;
    static cv::Mat frame;
    static cv::Mat frame2;
    static cv::Mat framecrop;
    static cv::Rect bbox;
    static cv::Rect bboxtmp;
    static bool ok_tr;
    static cv::Ptr<cv::TrackerCSRT> TrackerCSRT;
    static int window_x_start;
    static int window_y_start;
    static int window_x_size;
    static int window_y_size;
    static bool workingFlag;
    static bool trackingFlag;
    static int tick;
    static int frameCounter;
    static int putcounter;
    static int neurocounter;
    static std::time_t timeBegin;
    static std::string path1,path2,path3;
//    void myMouseCallBack( int event, int x, int y, int flags, void* param );
//    static void updating_tracker(uchar *pCurrentImageBuffer);

private:


public slots:
    void InitTracker(void);
//    void updating_tracker(QImage);
    void updating_tracker_uchar(uchar *);
    void bbox_update(int,int);

signals:
    void trackerframe(cv::Mat);
    void trackerframe2(QImage);
    void send_on_uart(int,int);
//    static void myMouseCallback(int event);

};

#endif // TRACKER_H

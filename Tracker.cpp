#include "Tracker.h"

cv::Ptr<cv::TrackerCSRT> Tracker::TrackerCSRT;
cv::Rect Tracker::bbox;
cv::Rect Tracker::bboxtmp;
QImage Tracker::imag;
bool Tracker::ok_tr;
bool Tracker::trackingFlag;
bool Tracker::workingFlag;
cv::Mat Tracker::frame;
cv::Mat Tracker::frame2;
cv::Mat Tracker::framecrop;
int Tracker::window_x_start;
int Tracker::window_y_start;
int Tracker::window_x_size;
int Tracker::window_y_size;
int Tracker::tick;
int Tracker::frameCounter;
int Tracker::putcounter;
int Tracker::neurocounter;
std::time_t Tracker::timeBegin;
RunYOLOonCPU Net;
std::string path1 = "C:/Users/nikit/Desktop/Trained_nets/20feb/brig2.names";
std::string path2 = "C:/Users/nikit/Desktop/Trained_nets/20feb/yolov4.cfg";
std::string path3 = "C:/Users/nikit/Desktop/Trained_nets/20feb/yolov4_best.weights";
std::vector<std::string> labels = Net.get_labels(path1);
bool gpu_run = 1;
auto model = Net.load_model(path2, path3, gpu_run);
float nms_thresh = 0.1;
float conf_thresh = 0.6;
bool show_output = 0;

Tracker::Tracker(QObject *parent) : QObject(parent)
{
    ok_tr  = false;
    trackingFlag = false;
    UART::Neuro_flag = 0;
}

void Tracker::InitTracker(void)
{
    TrackerCSRT = cv::TrackerCSRT::create();
    ok_tr  = false;
    trackingFlag = false;
    bbox = cv::Rect(100,100,10,10);
    bboxtmp = cv::Rect(100,100,10,10);
    window_x_size = 180;
    window_y_size = 180;
    workingFlag = true;
    frameCounter = 0;
    putcounter = 0;
    neurocounter = 0;
    tick = 0;
    timeBegin = std::time(0);
}



void Tracker::updating_tracker_uchar(uchar *pFrame)
{
    if (workingFlag == true)
    {
        workingFlag = false;

        if (UART::Neuro_flag == 1)
        {
            frame = cv::Mat(FRAME_HEIGHT, DOUBLE_FRAME_WIDTH, CV_8UC1, pFrame);
            cv::cvtColor(frame,frame2, cv::COLOR_GRAY2RGB);
            auto output = Net.get_prediction(frame2, &model, labels, nms_thresh, conf_thresh, show_output);
            auto [image, number_of_objects, box, scores] = output;
            if (box.size() != 0)
            {
                neurocounter++;
                int max = std::distance(scores.begin(),std::max_element(scores.begin(), scores.end()));
                cv::putText(frame,std::to_string(box[max].x),cv::Point(10,frame.rows / 2), cv::FONT_HERSHEY_DUPLEX,1.0,CV_RGB(118,185,0),2);

                if (UART::flight_indicator == 0 & neurocounter > 2)
                {
                    emit send_on_uart(((box[max].x + (box[max].width / 2)) - (DOUBLE_FRAME_WIDTH / 2)), (FRAME_HEIGHT / 2 - (box[max].y + (box[max].height / 2))));
                }
            }
            else
            {
                neurocounter = 0;
                emit send_on_uart(0,0);
            }
//            imshow("Tracker", frame);

            imag = QImage(image.data, image.cols, image.rows, image.step,  QImage::Format_RGB888);
            emit trackerframe2(imag);
        }
        else
        {
                frame = cv::Mat(FRAME_HEIGHT, DOUBLE_FRAME_WIDTH, CV_8UC1, pFrame);
        //        cv::resize(frame,frame, cv::Size(),2,2);

                if (window_x_start < 0)             {window_x_start = 0;}
                if (window_x_start > (frame.cols - window_x_size))   {window_x_start = frame.cols - window_x_size;}
                if (window_y_start < 0)             {window_y_start = 0;}
                if (window_y_start > (frame.rows - window_y_size))  {window_y_start = frame.rows - window_y_size;}

                cv::Rect region_of_interest = cv::Rect(window_x_start, window_y_start, window_x_size, window_y_size);
                framecrop = frame(region_of_interest);
        //        cv::rectangle(frame, region_of_interest, cv::Scalar(250,110,110), 2, 1);
                cv::putText(frame,std::to_string(putcounter),cv::Point(10,frame.rows / 2), cv::FONT_HERSHEY_DUPLEX,1.0,CV_RGB(118,185,0),2);
                if (trackingFlag)
                {
                   trackingFlag = TrackerCSRT->update(framecrop, bbox);
            //       if (trackingFlag == false) {ok_tr  = true;}
                   bboxtmp = cv::Rect((bbox.x+window_x_start),(bbox.y+window_y_start),40,40);
                   cv::rectangle(frame, bboxtmp, cv::Scalar(250,110,110), 2, 1);
                   double x = bboxtmp.x + 20 - (DOUBLE_FRAME_WIDTH / 2);
                   double y = (FRAME_HEIGHT / 2 - 40) - bboxtmp.y - 20;
                   if (UART::flight_indicator == 0)
                   {
                       emit send_on_uart((int)round(x), (int)round(y));
                   }
                   window_x_start = bboxtmp.x + 20 - window_x_size/2;
                   window_y_start = bboxtmp.y + 20 - window_y_size/2;
                }
                if (ok_tr)
                {
                    TrackerCSRT = cv::TrackerCSRT::create();
                    bbox = bboxtmp;
                    TrackerCSRT->init(framecrop, bbox);
                    ok_tr = false;
                    trackingFlag = true;
                }


                imag = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8);
        //        emit trackerframe(frame);
                emit trackerframe2(imag);
        //        imshow("Tracker", frame);
        //        // окно для отображения картинки
        //        cvNamedWindow("Tracker",0);
        //        // вешаем обработчик мышки
        //        cv::setMouseCallback( "Tracker", &myMouseCallback, 0);
                // fps
                frameCounter++;
                std::time_t timeNow = std::time(0) - timeBegin;
                if (timeNow - tick >= 1)
                {
                    tick++;
                    putcounter = frameCounter;
                    frameCounter = 0;
                }
        }
        workingFlag = true;
    }
}


void Tracker::bbox_update(int x,int y)
{
    window_x_start = x - window_x_size/2;
    window_y_start = y - window_y_size/2;
    bboxtmp = cv::Rect((window_x_size/2-20),(window_y_size/2-20),40,40);
    ok_tr = true;
    trackingFlag = false;
}


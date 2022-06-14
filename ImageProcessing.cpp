#include "ImageProcessing.h"

bool ImageProcessing::ModeFlag;
uchar ImageProcessing::TestOutputBufferCounter;
uchar ImageProcessing::InputBufferCounter, ImageProcessing::OutputBufferCounter;
uint ImageProcessing::FrameCounter;
bool ImageProcessing::IsAnyInputBufferFull;
bool ImageProcessing::IsInputBufferFull[BUFFER_NUMBER];
uchar ImageProcessing::InputBufferMask;
uchar *ImageProcessing::pImageBuffer[BUFFER_NUMBER];
uchar ImageProcessing::ImageBuffer[BUFFER_NUMBER][BUFFER_SIZE];
uchar ImageProcessing::WriteFrameBuffer[WRITE_BUFFER_SIZE];
QMutex ImageProcessing::Mutex_IP_S;
bool ImageProcessing::FrameRecordingFlag, ImageProcessing::TargetCoordinatesRecordingFlag;
QFile ImageProcessing::VideoFrameData("VideoFrameData.dat");
QFile ImageProcessing::TargetCoordinatesData("TargetCoordinatesData.dat");
//QTextStream ImageProcessing::VideoFrameDataStream(&VideoFrameData);
uchar ImageProcessing::ZoomFactor;
ushort ImageProcessing::HorizontalZoomCenter, ImageProcessing::VerticalZoomCenter;
QImage ImageProcessing::Image_IP(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB32);
//QImage ImageProcessing::Image_IP(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_Indexed8);
//uint *ImageProcessing::pStartImageData, *ImageProcessing::pImageData;
QTime ImageProcessing::ct;
cv::Mat ImageProcessing::frame;

ImageProcessing::ImageProcessing(QObject *parent) : QObject(parent)
{
    ModeFlag = false;
    TestOutputBufferCounter = 0;
    InputBufferCounter = 0;
    OutputBufferCounter = 0;
    FrameCounter = 0;
    IsAnyInputBufferFull = false;
    InputBufferMask = 0;
    ZoomFactor = 1;
    for (int i = 0; i < BUFFER_NUMBER; i++)
    {
        IsInputBufferFull[i] = false;
        pImageBuffer[i] = &ImageBuffer[i][0];
//        pImageBuffer[i] = (uchar *)ImageBuffer[i][0];
    }
    pTimer_IP = new QTimer();
    pTimer_IP->setInterval(5);
    pTimer_IP->start();
    connect(pTimer_IP, SIGNAL(timeout()), this, SLOT(Timer_IP_timeout()));
//    VideoFrameData("VideoFrameData.dat");
    VideoFrameData.open(QIODevice::WriteOnly);
    VideoFrameData.close();
//    VideoFrameDataStream(&VideoFrameData);

    TargetCoordinatesData.open(QIODevice::WriteOnly);
//    QTextStream TargetCoordinatesDataStream(&TargetCoordinatesData);
//    ct = QTime::currentTime();
//    TargetCoordinatesDataStream << "Time " << ct.toString("hh:mm:ss.zzz") << endl;
//    ct = QTime::currentTime();
//    TargetCoordinatesDataStream << "Time " << ct.toString("hh:mm:ss.zzz") << endl;
    TargetCoordinatesData.close();
//    pStartImageData = (uint *)(Image_IP.data_ptr());
//    pImageData = pStartImageData;
//    uint t = Image_IP.colorTable().size();
//    t = 256;
//    Image_IP.colorTable().resize(t);
//    t = Image_IP.colorTable().size();
    FrameRecordingFlag = false;
    TargetCoordinatesRecordingFlag = false;
}

void ImageProcessing::TestImages()
{
    uchar *pFrame, temp, col_step, row_step, ibc = 0;
    ushort row, col;
    TestOutputBufferCounter = 0;
    col_step = 32;
    row_step = 32;
    for (ibc = 0; ibc < BUFFER_NUMBER; ibc++)
    {
        pFrame = pImageBuffer[ibc];
        pFrame += DOUBLE_FRAME_HEIGHT;
        for (row = 0; row < FRAME_HEIGHT; row++)
        {
            for (col = 0; col < FRAME_WIDTH; col++)
            {
                temp = (col / col_step + row / row_step) % 2 ? 255 : 0;
                *pFrame++ = temp;
            }
        }
        col_step += 0;
        row_step += 0;
    }
}

uchar *ImageProcessing::GetInputBuffer()
{
    Mutex_IP.lock();
    uchar *pInputBuffer;
    InputBufferCounter++;
    if (InputBufferCounter >= BUFFER_NUMBER)
        InputBufferCounter = 0;
    pInputBuffer = pImageBuffer[InputBufferCounter];
    Mutex_IP.unlock();
    return pInputBuffer;
}

uchar *ImageProcessing::GetInputBufferS()
{
    Mutex_IP_S.lock();
    uchar *pInputBuffer;
//    if (InputBufferCounter < BUFFER_NUMBER)
//        pInputBuffer = pImageBuffer[InputBufferCounter];
//    else
//        pInputBuffer = pImageBuffer[0];
    InputBufferCounter++;
    if (InputBufferCounter >= BUFFER_NUMBER)
        InputBufferCounter = 0;
    pInputBuffer = pImageBuffer[InputBufferCounter];
    Mutex_IP_S.unlock();
    return pInputBuffer;
}

void ImageProcessing::SetIsInputBufferFull(uchar ibc)
{
    IsInputBufferFull[ibc] = true;
    InputBufferMask |= 1 << ibc;
//    for (int i = BUFFER_NUMBER - 1; i > 0; i--)

}

void ImageProcessing::Timer_IP_timeout()
{
    if (Form_VideoFrame::GetVideoViewingFlag())
    {
        //    MainFlags Flags = DataProcessing::GetMainFlags();
//        pImageData = pStartImageData;
        uint index;
        //    QTextStream VideoFrameDataStream(&VideoFrameData);
        //    QTextStream TargetCoordinatesDataStream(&TargetCoordinatesData);
        if (ModeFlag)
        {
            //        QImage Image_IP(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_RGB32);
            uchar *pFrame;
            ushort row, col;
            uchar temp;
            TestOutputBufferCounter++;
            if (TestOutputBufferCounter >= BUFFER_NUMBER)
                TestOutputBufferCounter = 0;
            pFrame = pImageBuffer[TestOutputBufferCounter];
            if (ZoomFactor == 1)
            {
                for (row = 0; row < IMAGE_HEIGHT; row++)
                {
                    for (col = 0; col < IMAGE_WIDTH; col++)
                    {
                        temp = *pFrame;
                        index = (0xffu << 24) | ((temp & 0xff) << 16) | ((temp & 0xff) << 8) | (temp & 0xff);
                        Image_IP.setPixel(col, row, index);
                        pFrame += FRAME_TO_IMAGE_SCALE;
                    }
                }
            }
            else
            {
                uchar row_cnt;
                ushort ZoomFrameWidth = FRAME_WIDTH / ZoomFactor;
                ushort ZoomFrameHeight = FRAME_HEIGHT / ZoomFactor;
                pFrame += (VerticalZoomCenter - ZoomFrameHeight / 2) * FRAME_WIDTH + HorizontalZoomCenter - ZoomFrameWidth / 2;
                for (row = 0; row < FRAME_HEIGHT; row += ZoomFactor)
                {
                    for (col = 0; col < FRAME_WIDTH; col++)
                    {
                        row_cnt = 0;
                        if (!(col % ZoomFactor))
                            temp = *pFrame++;
                        while (row_cnt < ZoomFactor)
                        {
                            index = (0xffu << 24) | ((temp & 0xff) << 16) | ((temp & 0xff) << 8) | (temp & 0xff);
                            Image_IP.setPixel(col, row + row_cnt, index);
                            row_cnt++;
                        }
                    }
                    pFrame += FRAME_WIDTH - ZoomFrameWidth;
                }
            }
            if (Tracker::workingFlag == true)
            {

                pFrame = &WriteFrameBuffer[0];
//                        emit onShowVideoFrame(Image_IP);
                emit onTracker(pFrame);
            }
        }
        else
        {
            if (InputBufferMask)
            {
                //            QImage Image_IP(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_RGB32);
                FrameCounter++;
                if (Image_IP.isNull())
                    return;
                uchar *pFrame, ibc, i;
                i = 0;
                ibc = InputBufferCounter;
                do
                {
                    ibc++;
                    if (ibc >= BUFFER_NUMBER)
                        ibc = 0;
                    i++;
                    if (i > BUFFER_NUMBER)
                    {
                        return;
                    }
                }
                while (!IsInputBufferFull[ibc]);
                OutputBufferCounter = ibc;
                pFrame = pImageBuffer[OutputBufferCounter];
                if (FrameRecordingFlag)
                {
                    //                frame = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1, pFrame);
                    emit onWriteVideoFrame(pFrame);
                }
                //            qDebug() << QTime::currentTime()
                //                     << "; pFrame: " << (uint)pFrame << "; InputBufferCounter: " << InputBufferCounter << "; InputBufferMask: " << InputBufferMask << "; OutputBufferCounter: " << OutputBufferCounter << "; i: " << i
                //                     << "; IsInputBufferFull: " << IsInputBufferFull[3] << " " << IsInputBufferFull[2] << " " << IsInputBufferFull[1] << " " << IsInputBufferFull[0];
                IsInputBufferFull[ibc] = false;
                InputBufferMask &= ~(1 << OutputBufferCounter);
                if (pFrame != NULL)
                {
                    ushort row, col;
                    uchar temp, index_8;
                    uint index;
                    if (ZoomFactor == 1)
                    {
                        //                    if (FrameRecordingFlag)
                        //                        VideoFrameData.open(QIODevice::Append);
                        for (row = 0; row < IMAGE_HEIGHT; row++)
                        {
//                            *pImageData++ = index;
                            for (col = 0; col < IMAGE_WIDTH; col++)
                            {
                                temp = *pFrame;
//                                index_8 = temp;
//                                index = (0xffu << 24) | ((temp & 0xff) << 16) | ((temp & 0xff) << 8) | (temp & 0xff);
                                index = ((temp & 0xff) << 16) | ((temp & 0xff) << 8) | (temp & 0xff);
//                                Image_IP.setPixel(col, row, index_8);
                                Image_IP.setPixel(col, row, index);
//                                *pImageData = index;
                                //                            if (FrameRecordingFlag)
                                //                                VideoFrameDataStream << temp << '\t';
                                //                            pFrame += FRAME_TO_IMAGE_SCALE;
                                pFrame++;
                            }
                            pFrame += (FRAME_TO_IMAGE_SCALE - 1) * FRAME_WIDTH;
                        }
                        //                    if (FrameRecordingFlag)
                        //                        VideoFrameData.close();
//                        index = (0xffu << 24) | ((0xFF) << 16) | ((0xFF) << 8) | (0x00);
                    }
                    else
                    {
                        uchar row_cnt;
                        uchar RealZoomFactor = ZoomFactor / FRAME_TO_IMAGE_SCALE;
                        ushort ZoomFrameWidth = FRAME_WIDTH / ZoomFactor;
                        ushort ZoomFrameHeight = FRAME_HEIGHT /ZoomFactor;
                        ushort  RealZoomFrameWidth = FRAME_WIDTH / RealZoomFactor;
                        ushort  RealZoomFrameHeight = FRAME_HEIGHT / RealZoomFactor;

                        pFrame += (VerticalZoomCenter - ZoomFrameHeight / 2) * FRAME_WIDTH + HorizontalZoomCenter - ZoomFrameWidth / 2;
                        for (row = 0; row < IMAGE_HEIGHT; row += RealZoomFactor)
                        {
                            for (col = 0; col < IMAGE_WIDTH; col++)
                            {
                                row_cnt = 0;
                                if (!(col % RealZoomFactor))
                                    temp = *pFrame++;
                                while (row_cnt < RealZoomFactor)
                                {
                                    index = (0xffu << 24) | ((temp & 0xff) << 16) | ((temp & 0xff) << 8) | (temp & 0xff);
                                    Image_IP.setPixel(col, row + row_cnt, index);
                                    row_cnt++;
                                }
                            }
                            pFrame += FRAME_WIDTH - ZoomFrameWidth;
                        }
                    }
                    if (Tracker::workingFlag == true)
                    {
                        pFrame = &WriteFrameBuffer[0];
//                        emit onShowVideoFrame(Image_IP);
                        emit onTracker(pFrame);
                    }
                }
            }
        }
    }
}

void ImageProcessing::SetZoomFactor(uchar zf, ushort hzc, ushort vzc)
{
    ZoomFactor = zf;
    if (zf == 1)
    {
        HorizontalZoomCenter = FRAME_WIDTH / 2;
        VerticalZoomCenter = FRAME_HEIGHT / 2;
    }
    else
    {
        ushort ZoomFrameWidth = FRAME_WIDTH / zf;
        ushort ZoomFrameHeight = FRAME_HEIGHT / zf;
        if (hzc < ZoomFrameWidth / 2)
            HorizontalZoomCenter = ZoomFrameWidth / 2;
        else
            if (FRAME_WIDTH - hzc < ZoomFrameWidth / 2)
                HorizontalZoomCenter = FRAME_WIDTH - ZoomFrameWidth / 2;
            else
                HorizontalZoomCenter = hzc;
        if (vzc < ZoomFrameHeight / 2)
            VerticalZoomCenter = ZoomFrameHeight / 2;
        else
            if (FRAME_HEIGHT - vzc < ZoomFrameHeight / 2)
                VerticalZoomCenter = FRAME_HEIGHT - ZoomFrameHeight / 2;
            else
                VerticalZoomCenter = vzc;
    }
}

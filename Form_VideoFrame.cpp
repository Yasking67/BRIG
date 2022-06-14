#include "Form_VideoFrame.h"
#include "ui_Form_VideoFrame.h"

using namespace std;
//using namespace cv;

bool Form_VideoFrame::VideoViewingFlag, Form_VideoFrame::VideoRecordingFlag, Form_VideoFrame::VideoIsOpened;
cv::VideoWriter Form_VideoFrame::writer;
//uint Form_VideoFrame::TimerCounter;
//QTime Form_VideoFrame::RecordTime, Form_VideoFrame::StartRecordTime;

Form_VideoFrame::Form_VideoFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_VideoFrame)
{
    ui->setupUi(this);

    pGraphicsScene_VF = new QGraphicsScene();
    ui->pGraphicsView->setScene(pGraphicsScene_VF);
//    this->setLayout(ui->pGridLayout);
    ui->pGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->pGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->pGraphicsView->viewport()->setMouseTracking(true);
    ui->pGraphicsView->viewport()->installEventFilter(this);

    pTimer_VF = new QTimer();
    pTimer_VF->setInterval(50);
    pTimer_VF->start();
    TimerCounter = 0;

//    connect(this, SIGNAL(onSendUDP_Packet(QByteArray)), UDP_Sender_BVR, SLOT(SendData(QByteArray)));
    connect(ui->pCheckBox_TestModeOnOff, SIGNAL(clicked()), this, SLOT(CheckBox_TestModeOnOff()));
    connect(this, SIGNAL(onUART_SendCommand_VF(uchar , uchar , uchar *)), SLOT(UART_SendCommand_VF(uchar , uchar , uchar *)));
    connect(ui->pRadioButton_ZoomFactor_x1, SIGNAL(clicked()), this, SLOT(RadioButton_ZoomFactor_x1_Click()));
    connect(ui->pRadioButton_ZoomFactor_x2, SIGNAL(clicked()), this, SLOT(RadioButton_ZoomFactor_x2_Click()));
    connect(ui->pRadioButton_ZoomFactor_x4, SIGNAL(clicked()), this, SLOT(RadioButton_ZoomFactor_x4_Click()));
    connect(ui->pRadioButton_ZoomFactor_x8, SIGNAL(clicked()), this, SLOT(RadioButton_ZoomFactor_x8_Click()));
    connect(ui->pRadioButton_ZoomFactor_x16, SIGNAL(clicked()), this, SLOT(RadioButton_ZoomFactor_x16_Click()));
    connect(pTimer_VF, SIGNAL(timeout()), this, SLOT(Timer_VF_timeout()));
    connect(ui->pCheckBox_VideoViewing, SIGNAL(clicked()), this, SLOT(CheckBox_VideoViewing()));
    connect(ui->pCheckBox_VideoRecording, SIGNAL(clicked()), this, SLOT(CheckBox_VideoRecording()));
    connect(ui->pRadioButton_VideoRecordFileName_Manual, SIGNAL(clicked()), this, SLOT(RadioButton_VideoRecordFileName_Manual_Click()));
    connect(ui->pRadioButton_VideoRecordFileName_Auto, SIGNAL(clicked()), this, SLOT(RadioButton_VideoRecordFileName_Auto_Click()));

    VideoRecordingFlag = false;
    frame = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1);
    int codec;
    QByteArray a;
    a.resize(4);
    char *pa;
    pa = a.data();

    for (int i = 0; i < supportedCodec.size(); i++)
    {
        a = supportedCodec[i].toUtf8();
        pa = a.data();
        codec = cv::VideoWriter::fourcc(pa[0], pa[1], pa[2], pa[3]);
        writer.open("test.avi", codec, 60.0, frame.size(), false);
        if (writer.isOpened())
        {
            availableCodecs.push_back(supportedCodec[i]);
            availableCodecsInt.push_back(codec);
            ui->pComboBox_CodecChoice->addItem(supportedCodec[i]);
        }
    }
    VideoViewingFlag = true;
    IsCompression = false;
}


Form_VideoFrame::~Form_VideoFrame()
{
    delete ui;
}

void Form_VideoFrame::CheckBox_TestModeOnOff()
{
    if (ui->pCheckBox_TestModeOnOff->isChecked())
    {
        ImageProcessing::TestImages();
        ImageProcessing::SetModeFlag(true);
    }
    else
        ImageProcessing::SetModeFlag(false);
}

void Form_VideoFrame::Timer_VF_timeout()
{
    TimerCounter++;
    if (VideoRecordingFlag && !(TimerCounter % 20))
    {
//        RecordTime.elapsed();
//        ui->pLabel_RecordTime->setText(RecordTime.toString("hh:mm:ss.zzz") + " (" +  QString::number(RecordTime.elapsed()) + ")");
//        ui->pLabel_RecordTime->setText(RecordTime.elapsed().toString("hh:mm:ss.zzz"));
//        RecordTime = StartRecordTime.addMSecs(StartRecordTime.elapsed());
        ui->pLabel_RecordTime->setText(RecordTime.addMSecs(StartRecordTime.elapsed()).toString("hh:mm:ss.zzz"));
//        ui->pLabel_RecordTime->setText(RecordTime.toString("hh:mm:ss.zzz") + " (" +  QString::number(StartRecordTime.elapsed()) + ")");
    }
}

void Form_VideoFrame::DrawNewFrame(QImage img)
{
//    QImage Image(this->size(),QImage::Format_RGB32);
//    QImage Image(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_RGB32);
//    Image = img.scaled(ui->pGraphicsView->size(), Qt::KeepAspectRatio);
    if (VideoViewingFlag)
    {
        pGraphicsScene_VF->clear();
        pGraphicsScene_VF->addPixmap(QPixmap::fromImage(img));
    }
}

void Form_VideoFrame::DrawNewFrame2(QImage img)
{
//    QImage Image(this->size(),QImage::Format_RGB32);
//    QImage Image(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_RGB32);
//    Image = img.scaled(ui->pGraphicsView->size(), Qt::KeepAspectRatio);
    if (VideoViewingFlag)
    {
        pGraphicsScene_VF->clear();
        pGraphicsScene_VF->addPixmap(QPixmap::fromImage(img));
    }
}

void Form_VideoFrame::WriteNewFrame(uchar *pFrame)
{
    if (VideoRecordingFlag && VideoIsOpened)
    {
        cv::Mat frm = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1, pFrame);
        writer.write(frm);
    }
}

void Form_VideoFrame::WriteNewFrame2(cv::Mat frame)
{
    if (VideoRecordingFlag && VideoIsOpened)
    {
        writer.write(frame);
    }
}

//void Form_VideoFrame::mouseMoveEvent(QMouseEvent *event)
//{
//    QPoint p = event->pos();
////    QPoint p = QCursor::pos();
//    ui->pLabel_MouseCursorPosition->setText(QString("Mouse: X = %1").arg(p.x()) + QString("; Y = %1").arg(p.y()));
//}

//void Form_VideoFrame::hoverMoveEvent(QMouseEvent *event)
//{
//    QPoint p = event->pos();
////    QPoint p = QCursor::pos();
//    ui->pLabel_MouseCursorPosition->setText(QString("Hover: X = %1").arg(p.x()) + QString("; Y = %1").arg(p.y()));
//}

bool Form_VideoFrame::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress)
    {
        if (event->type() == QEvent::MouseMove)
        {
////          qDebug() << event;
////          QMouseEvent *pMouseEvent = static_castevent;
//            QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);
////          QPoint p = pMouseEvent->pos();
//            QPointF p = pMouseEvent->localPos();
////          QPoint p = (QMouseEvent)event->localPos();
//            ushort col = p.x(), row = p.y();
//            uchar *pBuffer;
//            uchar zf = ImageProcessing::GetZoomFactor();
//            ushort hzc = ImageProcessing::GetHorizontalZoomCenter();
//            ushort vzc = ImageProcessing::GetVerticalZoomCenter();
//            if (ImageProcessing::GetModeFlag())
//            {
//                if (zf == 1)
//                    pBuffer = ImageProcessing::GetTestOutputBuffer() + DOUBLE_FRAME_HEIGHT + row * FRAME_WIDTH + col;
//                else
//                {
//                    ushort ZoomFrameWidth = FRAME_WIDTH / zf;
//                    ushort ZoomFrameHeight = FRAME_HEIGHT / zf;
//                    pBuffer = ImageProcessing::GetTestOutputBuffer() + DOUBLE_FRAME_HEIGHT + (vzc - ZoomFrameHeight / 2) * FRAME_WIDTH + hzc - ZoomFrameWidth / 2 + (row / zf) * FRAME_WIDTH + col / zf;
//                }
//            }
//            else
//            {
//                if (zf > 1)
//                {
//                    ushort ZoomFrameWidth = FRAME_WIDTH / zf;
//                    ushort ZoomFrameHeight = FRAME_HEIGHT / zf;
//                    col = hzc - ZoomFrameWidth / 2 + col / zf;
//                    row = vzc - ZoomFrameHeight / 2 + row / zf;
//                }
//                pBuffer = ImageProcessing::GetOutputBufferS() + DOUBLE_FRAME_HEIGHT + row * FRAME_WIDTH * FRAME_TO_IMAGE_SCALE + col * FRAME_TO_IMAGE_SCALE;
//            }
//            uchar Exposition = *pBuffer;
//            ui->pLabel_MouseCursorPosition->setText(QString("X = %1").arg(col) + QString("; Y = %1").arg(row) + "; E = " + QString::number(Exposition));
        }
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);
            QPointF p = pMouseEvent->localPos();
            ui->pLineEdit_HorizontalZoomCenter->setText(QString::number(p.x(), 'f', 0));
            ui->pLineEdit_VerticalZoomCenter->setText(QString::number(p.y(), 'f', 0));
            int px = p.x();
            int py = p.y();
            emit update_bbox(px,py);
            emit send_on_camera(px, py);
        }
        return true;
    }
    return false;
}

void Form_VideoFrame::UART_SendCommand_VF(uchar preamble, uchar size, uchar *data)
{
    uchar i, j, PacketNumber, PacketSize, DataOffset, *pData;
    QByteArray UDP_SendPacketArray;
    PacketNumber = size / (SEND_DATA_SIZE - 4) +  (uchar)((size % (SEND_DATA_SIZE - 4)) && 1);

    if ((preamble == 0x51 || preamble == 0x55) && data != NULL)
    {
        DataOffset = 0;
        UDP_SendPacketArray.resize(SEND_DATA_SIZE);
        UDP_SendPacketArray[0] = 0x01;
        UDP_SendPacketArray[1] = 0x00;
        UDP_SendPacketArray[2] = preamble;
        for (j = 0; j < PacketNumber; j++)
        {
            if (j)
                QThread::usleep(2000);
            PacketSize = (size - (SEND_DATA_SIZE - 4) * j > SEND_DATA_SIZE - 4) ? (SEND_DATA_SIZE - 4) : (size - (SEND_DATA_SIZE - 4) * j);
            UDP_SendPacketArray[3] = PacketSize + 2;
            pData = data + (SEND_DATA_SIZE - 4) * j;
            for (i = 0; i < PacketSize; i++)
                UDP_SendPacketArray[i + 4] = *pData++;
            emit onSendUDP_Packet(UDP_SendPacketArray);
        }
    }
}

void Form_VideoFrame::RadioButton_ZoomFactor_x1_Click()
{
    GroupBox_ZoomFactor_Click(1);
}
void Form_VideoFrame::RadioButton_ZoomFactor_x2_Click()
{
    GroupBox_ZoomFactor_Click(2);
}
void Form_VideoFrame::RadioButton_ZoomFactor_x4_Click()
{
    GroupBox_ZoomFactor_Click(4);
}
void Form_VideoFrame::RadioButton_ZoomFactor_x8_Click()
{
    GroupBox_ZoomFactor_Click(8);
}
void Form_VideoFrame::RadioButton_ZoomFactor_x16_Click()
{
    GroupBox_ZoomFactor_Click(16);
}
void Form_VideoFrame::GroupBox_ZoomFactor_Click(uchar zf)
{
    ushort hzc = ui->pLineEdit_HorizontalZoomCenter->text().toUShort();
    ushort vzc = ui->pLineEdit_VerticalZoomCenter->text().toUShort();
    ImageProcessing::SetZoomFactor(zf, hzc, vzc);
    ui->pLineEdit_HorizontalZoomCenter->setText(QString::number(ImageProcessing::GetHorizontalZoomCenter()));
    ui->pLineEdit_VerticalZoomCenter->setText(QString::number(ImageProcessing::GetVerticalZoomCenter()));
}

void Form_VideoFrame::CheckBox_VideoViewing()
{
    if (ui->pCheckBox_VideoViewing->isChecked())
    {
        VideoViewingFPS = ui->pLineEdit_VideoViewingFPS->text().toInt();
        VideoViewingFlag = true;
    }
    else
    {
        VideoViewingFlag = false;
    }
}

void Form_VideoFrame::CheckBox_VideoRecording()
{
    if (ui->pCheckBox_VideoRecording->isChecked())
    {
        VideoRecordingFlag = true;
        StartRecordTime = QTime::currentTime();
        ui->pLabel_StartRecordTime->setText(StartRecordTime.toString("hh:mm:ss"));
        StartRecordTime.start();
        RecordTime.setHMS(0, 0, 0);
        ui->pLabel_RecordTime->setText(RecordTime.toString("hh:mm:ss.zzz"));
        ui->pLabel_RecordTime->setStyleSheet("color : red");
        if (ui->pRadioButton_VideoRecordFileName_Manual->isChecked())
            VideoRecordFileName = ui->pLineEdit_VideoRecordFileName_Manual->text().toStdString();
        else
            VideoRecordFileName = ui->pLineEdit_VideoRecordFileName_Auto->text().toStdString() + StartRecordTime.toString("hhmmss").toStdString() + ".avi";
//        int codec = cv::VideoWriter::fourcc('H', '2', '6', '4');  // select desired codec (must be available at runtime)
////        codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
//        codec = cv::VideoWriter::fourcc('M', 'P', 'G', '4');
////        codec = VideoWriter::fourcc('X', 'V', 'I', 'D');
////        codec = VideoWriter::fourcc('M', 'P', '4', '2');
//        codec = cv::VideoWriter::fourcc('M', 'P', '4', '3');
        int codec = 0;
        double fps = 60;                          // framerate of the created video stream
        if (ui->pRadioButton_Compression_Yes->isChecked())
        {
            int index = ui->pComboBox_CodecChoice->currentIndex();
            codec = availableCodecsInt[index];
        }
        bool isColor = false;
        writer.open(VideoRecordFileName, codec, fps, frame.size(), isColor);
        ui->pLabel_VideoRecordFileName->setText(QString::fromStdString(VideoRecordFileName));
        if (!writer.isOpened())
        {
            qDebug() << "Could not open the output video file for write\n";
        }
    }
    else
    {
        writer.release();
        VideoRecordingFlag = false;
        ui->pLabel_RecordTime->setStyleSheet("color : black");
    }
}

void Form_VideoFrame::RadioButton_VideoRecordFileName_Manual_Click()
{
//        ui->pLineEdit_VideoRecordFileName_Manual->isEnabled();
}

void Form_VideoFrame::RadioButton_VideoRecordFileName_Auto_Click()
{
//        ui->pLineEdit_VideoRecordFileName_Auto->isEnabled();
}

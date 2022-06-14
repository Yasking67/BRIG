#include "MainWindow.h"
#include "ui_MainWindow.h"

//тестовая запись для проверки гитхаб

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    UDP_Sender_BVR = new UDP_Sender();
    UDP_Receiver_BVR = new UDP_Receiver();

    UDP_SenderThread = new QThread();
    UDP_ReceiverThread = new QThread();

    UDP_Sender_BVR->moveToThread(UDP_SenderThread);
    UDP_Receiver_BVR->moveToThread(UDP_ReceiverThread);

    Form_VideoFrame_BVR = new Form_VideoFrame();

    UDP_TrackerThread = new QThread();
    Tracker_BVR = new Tracker();
    Tracker_BVR->moveToThread(UDP_TrackerThread);

    UART_BVR = new UART();
    UART_BVR->setWindowIcon(QIcon("D:/Work/Бриг/plane.png"));
    UART_BVR->show();

    pTimer_MW = new QTimer();
    pTimer_MW->setInterval(50);
    pTimer_MW->start();
    TimerCounter = 0;
    FlagsCounter = 0;

    ImageProcessing_BVR = new ImageProcessing();
    ImageProcessingThread = new QThread();
    ImageProcessing_BVR->moveToThread(ImageProcessingThread);

    DataProcessing *DataProcessing_BVR = new DataProcessing();

    connect(this, SIGNAL(onWriteDescriptor(QString)), this, SLOT(WriteDes(QString)));

    connect(ui->pPushButton_DeviceInitialization, SIGNAL(clicked()), UDP_SenderThread, SLOT(start()));
    connect(ui->pPushButton_DeviceInitialization, SIGNAL(clicked()), UDP_ReceiverThread, SLOT(start()));
    connect(ui->pPushButton_DeviceInitialization, SIGNAL(clicked()), ImageProcessingThread, SLOT(start()));
    connect(ui->pPushButton_DeviceInitialization, SIGNAL(clicked()), UDP_TrackerThread, SLOT(start()));
//    connect(ui->pPushButton_DeviceInitialization, SIGNAL(clicked()), UDP_TrackerThread, SLOT(start()));

    connect(UDP_ReceiverThread, SIGNAL(started()), this, SLOT(InitPCAP()));

    connect(UDP_SenderThread, SIGNAL(started()), UDP_Sender_BVR, SLOT(InitUDP_Sender()));
    connect(this, SIGNAL(onInitPCAP(QString)), UDP_Receiver_BVR, SLOT(InitUDP_Receiver(QString)));
//    connect(ImageProcessingThread, SIGNAL(started()),ImageProcessing_BVR, SLOT(ImageProcessing()));

    connect(ui->pPushButton_Start, SIGNAL(clicked()), UDP_Sender_BVR, SLOT(StartVideo()));
    connect(ui->pPushButton_Stop, SIGNAL(clicked()), UDP_Sender_BVR, SLOT(StopVideo()));
    connect(ui->pPushButton_SendCommand, SIGNAL(clicked()), this, SLOT(PushButton_SendCommand_Click()));
    connect(this, SIGNAL(onSendUDP_Packet(QByteArray)), UDP_Sender_BVR, SLOT(SendData(QByteArray)));
    connect(this, SIGNAL(onUART_SendCommand(uchar, uchar, QByteArray)), UDP_Sender_BVR, SLOT(UART_SendCommand(uchar, uchar, QByteArray)));
    connect(this, SIGNAL(onSendUDP_PacketHost(QByteArray, QByteArray, ushort)), UDP_Sender_BVR, SLOT(SendDataHost(QByteArray, QByteArray, ushort)));
    connect(this, SIGNAL(onSendUDP_PacketHost(QByteArray, uint, ushort)), UDP_Sender_BVR, SLOT(SendDataHost(QByteArray, uint, ushort)));

    connect(ui->pPushButton_Start, SIGNAL(clicked()), Form_VideoFrame_BVR, SLOT(show()));
    connect(ui->pPushButton_Stop, SIGNAL(clicked()), Form_VideoFrame_BVR, SLOT(close()));

    connect(this, SIGNAL(onUART_SendCommand(uchar , uchar , uchar *)), SLOT(UART_SendCommand(uchar , uchar , uchar *)));

    connect(UDP_Receiver_BVR, SIGNAL(onUpdateFrameCounter(unsigned char)), this, SLOT(UpdateFrameCounter(unsigned char)), Qt::DirectConnection);
    connect(UDP_Receiver_BVR, SIGNAL(onPrintFlags(unsigned int)), this, SLOT(PrintFlags(unsigned int)));

    connect(pTimer_MW, SIGNAL(timeout()), this, SLOT(Timer_MW_timeout()));

//    connect(ImageProcessing_BVR, SIGNAL(onShowVideoFrame(QImage)), Form_VideoFrame_BVR, SLOT(DrawNewFrame(QImage)));
    connect(UDP_TrackerThread, SIGNAL(started()), Tracker_BVR, SLOT(InitTracker()));
//      connect(ImageProcessing_BVR, SIGNAL(onShowVideoFrame(QImage)), Tracker_BVR, SLOT(updating_tracker(QImage)));
      connect(ImageProcessing_BVR, SIGNAL(onTracker(uchar *)), Tracker_BVR, SLOT(updating_tracker_uchar(uchar *)));
//    connect(ImageProcessing_BVR, SIGNAL(onWriteVideoFrame(uchar *)), Form_VideoFrame_BVR, SLOT(WriteNewFrame(uchar *)));
    connect(UDP_Receiver_BVR, SIGNAL(onTracker(uchar *)), Tracker_BVR, SLOT(updating_tracker(uchar *)));
      connect(Form_VideoFrame_BVR, SIGNAL(update_bbox(int,int)), Tracker_BVR, SLOT(bbox_update(int,int)));
      connect(Form_VideoFrame_BVR, SIGNAL(send_on_camera(int,int)), UART_BVR, SLOT(SendOnCamera(int,int)));

    connect(Tracker_BVR, SIGNAL(trackerframe2(QImage)), Form_VideoFrame_BVR, SLOT(DrawNewFrame(QImage)));
    connect(Tracker_BVR, SIGNAL(trackerframe(cv::Mat)), Form_VideoFrame_BVR, SLOT(WriteNewFrame2(cv::Mat frame)));
    connect(Tracker_BVR, SIGNAL(send_on_uart(int,int)), UART_BVR, SLOT(Send(int,int)));

    connect(ui->pCheckBox_FrameRecording, SIGNAL(clicked()), this, SLOT(CheckBox_FrameRecording()));

    connect(Form_VideoFrame_BVR, SIGNAL(onSendUDP_Packet(QByteArray)), UDP_Sender_BVR, SLOT(SendData(QByteArray)));


    // Find Adapters //
    pcap_if_t *alldevs;
    pcap_if_t *d;
    char errbuf[PCAP_ERRBUF_SIZE];
    // Get the device list
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        //QMessageBox.warning(this,"Критическая ошибка", "Ошибка в поиске адаптеров");
       // qDebug()<<stderr<<" Error in findalldevs: "<< errbuf;
        exit(1);
    }

    int i = 0;
    //print the list
    for (d = alldevs; d; d = d->next)
    {

        if (d->description)
        {
            ++i;
            emit onWriteDescriptor(d->description);
        }
    }
    if (i == 0)
    {
        emit onWriteDescriptor("No devices found");
        pcap_freealldevs(alldevs);
    }

    PreviousFrameCounter = 0;
    FPS_Time.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::WriteDes(QString descr)
{
    ui->pComboBox_DeviceChoice->addItem(descr);
}

void MainWindow::InitPCAP()
{
    emit onInitPCAP(ui->pComboBox_DeviceChoice->currentText());
    ui->pPushButton_Start->setEnabled(true);
    ui->pPushButton_Stop->setEnabled(true);
}

void MainWindow::UpdateFrameCounter(unsigned char fc)
{
    ui->pLabel_FPS->setText(QString::number(fc));
}

void MainWindow::PushButton_SendCommand_Click()
{
    QString address;
    QString command;
//    static QByteArray AddressArray;
//    static QByteArray CommandArray;
    QByteArray TempArray;
    QByteArray UDP_SendPacketArray;
    QByteArray Data;
    Data.resize(SEND_DATA_SIZE);
    Data[0] = 0x01;
    Data[1] = 0x00;
    Data[2] = 0xAA;
    Data[3] = 0x00;

//    AddressArray.fill(0, 2);
//    CommandArray.fill(0, 4);
    UDP_SendPacketArray.fill(0, 2);

    address.append(ui->pLineEdit_Address->text());
//    UDP_SendPacketArray.append(ReadHEX(address));
    command.append(ui->pLineEdit_Command->text());
//    UDP_SendPacketArray.append(ReadHEX(command));
//    TempArray = QByteArray::fromHex("ABCD");

    if (address[1] == 'x')
        address.remove(0, 2);
    if (command[1] == 'x')
        command.remove(0, 2);
    TempArray = QByteArray::fromHex(address.toLatin1());
    Data[4] = TempArray[0];
    Data[5] = TempArray[1];
    UDP_SendPacketArray.append(TempArray);
    TempArray = QByteArray::fromHex(command.toLatin1());
    Data[6] = TempArray[0];
    Data[7] = TempArray[1];
    Data[8] = TempArray[2];
    Data[9] = TempArray[3];
    UDP_SendPacketArray.append(TempArray);
    TempArray = QByteArray::fromHex("ABCD");
    emit onSendUDP_Packet(Data);
}

void MainWindow::SendCommand_Click(ushort address, uint command)
{
    QByteArray Data;
    Data.resize(SEND_DATA_SIZE);
    Data[0] = 0x01;
    Data[1] = 0x00;
    Data[2] = 0xAA;
    Data[3] = 0x00;
    Data[4] = (uchar)(address >> 8);
    Data[5] = (uchar)address;
    Data[6] = (uchar)(command >> 24);
    Data[7] = (uchar)(command >> 16);
    Data[8] = (uchar)(command >> 8);
    Data[9] = (uchar)command;
    emit onSendUDP_Packet(Data);
}

void MainWindow::Timer_MW_timeout()
{
    static uchar BufferNumber, BufferNumberS, *pBuffer, *pBufferS;
    QString str;
    TimerCounter++;
    uint FlagsAll;
    MainFlags Flags;
//    ui->pLabel_Flags->setText("Флаги (Flags): " + Converts::IntToBinStr(TimerCounter, 32, 8));
    if (!(TimerCounter % 20))
    {
        CurrentFrameCounter = ImageProcessing::GetFrameCounterS();
        ui->pLabel_TimerCounter->setText(QString::number(TimerCounter));
        ui->pLabel_FrameCounter->setText(QString::number(CurrentFrameCounter));
        uint t = FPS_Time.elapsed();
        FPS_Time.restart();
        float FPS = 1000.0 * (CurrentFrameCounter - PreviousFrameCounter) / t;
        ui->pLabel_FPS->setText(QLocale().toString(FPS, 'f', 1) + " (" + QString::number(CurrentFrameCounter - PreviousFrameCounter) + ")");
        PreviousFrameCounter = CurrentFrameCounter;
    }

    if (!(TimerCounter % 5))
    {
        if (DataProcessing::ReceivingNewDataFlagsDP.AnyReceiving)
        {
            if (DataProcessing::ReceivingNewDataFlagsDP.SettingsReceiving)
            {
                FlagsAll = DataProcessing::GetMainFlagsAll();
                Flags = DataProcessing::GetMainFlags();
//                ui->pLabel_Flags->setText("Флаги (Flags): " + Converts::IntToBinStr(Flags.all, 24, 8) + " (" + QString::number(FlagsCounter++) + ")");
                DataProcessing::ReceivingNewDataFlagsDP.SettingsReceiving = 0;
            }
        }
    }
}

void MainWindow::UART_SendCommand(uchar preamble, uchar size, uchar *data)
{
    uchar i, j, PacketNumber, PacketSize, DataOffset, *pData;
    QByteArray UDP_SendPacketArray;
    PacketNumber = size / (SEND_DATA_SIZE - 4) +  (uchar)((size % (SEND_DATA_SIZE - 4)) && 1);

    if ((preamble == SENDING_PREAMBLE_0 || preamble == SENDING_PREAMBLE_1) && data != NULL)
    {
        DataOffset = 0;
        UDP_SendPacketArray.resize(SEND_DATA_SIZE);
        UDP_SendPacketArray[0] = (uchar)(CONTROL_UDP_PREAMBLE >> 8);
        UDP_SendPacketArray[1] = (uchar)CONTROL_UDP_PREAMBLE;
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

void MainWindow::PrintFlags(unsigned int f)
{
//    ui->pLabel_Flags->setText("Флаги (Flags): " + QString::number(f));
}

void MainWindow::CheckBox_FrameRecording()
{
    if (ui->pCheckBox_FrameRecording->isChecked())
        ImageProcessing::FrameRecordingFlag = true;
    else
        ImageProcessing::FrameRecordingFlag = false;
}

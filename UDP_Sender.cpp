#include "UDP_Sender.h"

UDP_Sender::UDP_Sender(QObject *parent) : QObject(parent)
{

}

void UDP_Sender::InitUDP_Sender()
{
    socket = new QUdpSocket();
    socket->bind(DataProcessing::GetCCP_Port(), QUdpSocket::ShareAddress);
    qDebug() << "Init UDP Tx: " << QThread::currentThread();
}

void UDP_Sender::SetCCP_Port(ushort port)
{
    socket->reset();
//    socket->close();
//    socket->waitForDisconnected(100);
//    socket->abort();
    quint16 localport1 = 0;
    socket->bind(port, QUdpSocket::ShareAddress);
    quint16 localport = socket->localPort();
    localport1 = localport;
}

void UDP_Sender::SendData(QByteArray array)
{
    uint HostIP_Address = DataProcessing::GetDeviceIP_Address();
//    ushort port = DataProcessing::GetCCP_Port();
    ushort port = DataProcessing::GetDevicePort();
    QHostAddress *HostAddress;
    HostAddress = new QHostAddress(HostIP_Address);
    socket->writeDatagram(array, array.size(), *HostAddress, port);
}

void UDP_Sender::UART_SendCommand(uchar preamble, uchar size, QByteArray array)
{
    uint HostIP_Address = DataProcessing::GetDeviceIP_Address();
    ushort port = DataProcessing::GetDevicePort();
    QHostAddress *HostAddress;
    HostAddress = new QHostAddress(HostIP_Address);
    uchar i, j, PacketNumber, PacketSize;
    QByteArray UDP_SendPacketArray;
    PacketNumber = size / (SEND_DATA_SIZE - 4) +  (uchar)((size % (SEND_DATA_SIZE - 4)) && 1);
    if (preamble == SENDING_PREAMBLE_0 || preamble == SENDING_PREAMBLE_1)
    {
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
            for (i = 0; i < PacketSize; i++)
                UDP_SendPacketArray[i + 4] = array[(SEND_DATA_SIZE - 4) * j + i];
            socket->writeDatagram(array, array.size(), *HostAddress, port);
        }
    }
}

void UDP_Sender::SendDataHost(QByteArray array, QByteArray address, ushort port)
{
    QHostAddress *HostAddress;
    HostAddress = new QHostAddress(((uchar)address[0] << 24) + ((uchar)address[1] << 16) + ((uchar)address[2] << 8) + (uchar)address[3]);
//    HostAddress = new QHostAddress((192 << 24) + (168 << 16) + (0 << 8) + 225);
    qint64 result = socket->writeDatagram(array, array.size(), *HostAddress, port);
//    HostAddress = new QHostAddress((255 << 24) + (255 << 16) + (255 << 8) + 255);
//    result = socket->writeDatagram(array, array.size(), *HostAddress, 52516);
//    result = socket->writeDatagram(array, array.size(), QHostAddress::Broadcast, 52516);
//    qint64 result1 = result;
}

void UDP_Sender::SendDataHost(QByteArray array, uint IP_Address, ushort port)
{
    QHostAddress *HostAddress;
    HostAddress = new QHostAddress(IP_Address);
    qint64 result = socket->writeDatagram(array, array.size(), *HostAddress, port);
}

void UDP_Sender::StartVideo()
{
    QByteArray Data;
    Data.resize(SEND_DATA_SIZE);
    Data[0] = 0x01;
    Data[1] = 0x00;
    Data[2] = 0xAA;
    Data[3] = 0x00;
    Data[4] = 0x01;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x00;
    Data[8] = 0x00;
    Data[9] = 0x01;
    socket->writeDatagram(Data, Data.size(), QHostAddress::Broadcast, 52516);
}

void UDP_Sender::StopVideo()
{
    QByteArray Data;
    Data.resize(SEND_DATA_SIZE);
    Data[0] = 0x01;
    Data[1] = 0x00;
    Data[2] = 0xAA;
    Data[3] = 0x00;
    Data[4] = 0x01;
    Data[5] = 0x00;
    Data[6] = 0x00;
    Data[7] = 0x00;
    Data[8] = 0x00;
    Data[9] = 0x00;
    socket->writeDatagram(Data, Data.size(), QHostAddress::Broadcast, 52516);
}


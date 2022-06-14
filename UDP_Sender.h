#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include <QObject>
#include <QByteArray>
#include <QUdpSocket>
#include <QThread>
#include <windows.h>
//#include <QDebug>

#include "DataProcessing.h"

#define SEND_DATA_SIZE                                      22

class UDP_Sender : public QObject
{
    Q_OBJECT
public:
    explicit UDP_Sender(QObject *parent = 0);

private:
    QUdpSocket *socket;

signals:

public slots:
    void InitUDP_Sender(void);
    void SendData(QByteArray);
    void UART_SendCommand(uchar preamble, uchar size, QByteArray array);
    void SendDataHost(QByteArray, QByteArray, ushort);
    void SendDataHost(QByteArray, uint, ushort);
    void StartVideo(void);
    void StopVideo(void);
    void SetCCP_Port(ushort port);
};

#endif // UDP_SENDER_H

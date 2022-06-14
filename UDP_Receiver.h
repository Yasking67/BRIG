#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include <QObject>
#include <QByteArray>
#include <QIODevice>
#include <stdio.h>
#include <winsock2.h>
#include <pcap.h>
#include <iphlpapi.h>
#include <QDebug>
#include <QThread>
#include <QTimer>
//#include "mem.h"

//#include "MainWindow.h"
#include "ImageProcessing.h"
#include "DataProcessing.h"
#include "Converts.h"
#include "Form_VideoFrame.h"

#define IPTOSBUFFERS 12
// bytes swap
#define HTYPE 0x0100
#define PTYPE 0x0008
#define FTYPE 0x0608
#define ARP_REQUEST 0x0100
#define HLEN_PLEN 0x0406
#define OPERATION 0x0100
#define IP_TYPE 0x0008

#define PACKET_FLAG_INDEX 3                                           //1 - new version, 3 - old version
#define PACKET_SIGNATURE_INDEX 2
#define UART_DATA_SIZE_INDEX 3
#define UART_DATA_START_INDEX 4
#define PACKET_FLAG 0x00
#define SERVICE_FRAME_PACKET_FLAG 0x01
#define FRAME_PACKET_FLAG 0x02
#define TARGETS_PACKET_FLAG 0x04
#define CCP_PACKET_SIZE 64
#define SERVICE_FRAME_PACKET_SIZE 64                                  //64 - new version, 60 - old version
#define FRAME_PACKET_SIZE 1286
#define FRAME_PACKET_FULL_SIZE 3888
#define FRAME_PACKET_NUMBER 270
#define UART_PACKET_SIZE 64

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#define WORKING_BUFFER_SIZE 15000

class UDP_Receiver : public QObject
{
    Q_OBJECT

public:
    explicit UDP_Receiver(QObject *parent = 0);
    ~UDP_Receiver();

    static UDP_Receiver* getInstance();

    QTimer *timer;

    static bool FLAG_WRITE_READY;
    //4 bytes IP address;
    typedef struct ip_address
    {
        u_char byte1;
        u_char byte2;
        u_char byte3;
        u_char byte4;

    } ip_address;

    typedef struct mac_address
    {
        u_char byte1;
        u_char byte2;
        u_char byte3;
        u_char byte4;
        u_char byte5;
        u_char byte6;

    } mac_address;

    typedef struct arp_header
    {
        mac_address dest_mac_broadcast; // destination mac address (0xFFFFFFFFFFFF)
        mac_address src_mac0; // source mac address
        u_short     ftype; // Frame type 0x0806
        u_short     htype; // Hardware type 0x0001
        u_short     ptype; // Protocol type 0x0800
        u_char      hlen; // MAC lenth 0x06
        u_char      plen; // IP lenth 0x04
        u_short      arp_cmd; // Arp request(0x0001), arp answer(0x0002)
        mac_address src_mac; // source MAC
        ip_address  src_ip; // source IP
        mac_address dst_mac; // destination MAC
        ip_address  dst_ip; // destination IP


    } arp_header;

    typedef struct ip_header
    {
        u_char ver_ihl;         // version (4 bits0 + Internet header length (4 bits)
        u_char tos;             // type of service
        u_short tlen;           // total length
        u_short identification; // Identification
        u_short flags_fo;       // flags (3 bits) + fragment offset (13 bits)
        u_char ttl;             // time to live;
        u_char proto;           // protocol
        u_short crc;			// Header checksum
        ip_address	saddr;		// Source address
        ip_address	daddr;		// Destination address
        u_int	op_pad;			// Option + Padding
    } ip_header;

    // UDP header
    typedef struct udp_header
    {
        u_short sport;			// Source port
        u_short dport;			// Destination port
        u_short len;			// Datagram length
        u_short crc;			// Checksum
        u_char datagram[FRAME_PACKET_SIZE]; // поменял с char на u_char
    } udp_header;
/*
    typedef struct TPacket
    {
        char  ethHeader;
        char  ipHeader;
        short portSrc;
        short portDst;
        short len;
        short checkSum;
        char  data[1328];
    }TPacket;
*/
    void CreatePacket(QString Pckt_type,
                      BYTE* SourceMAC,
                      BYTE* DestinationMAC,
                      BYTE*   SourceIP,
                      BYTE*   DestIP,
                      unsigned short SourcePort,
                      unsigned short DestinationPort,
                      unsigned char UserData,
                      unsigned int   UserDataLen);
    void SendPcktPcapUdp(QByteArray Data, int len);

/*
protected:

    UDP_Receiver();
*/
private:

    // static pcap_t *adhandle;
    //prototype of the packet handler
   static void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
   void emitBufRdy(unsigned char *buf, unsigned char *Temp);
   void emitIP_MAC_recieve(QList<ip_address> *ip, QList<mac_address> *mac);
   void emitEchoRdy(unsigned char *buf);
   void emitImgCalibView(unsigned char *buf, unsigned char *Temp);
   void emitRowReciveCheck(unsigned char *R);
   void emitUpdateFrameCounter(unsigned char fc);
 //  static UDP_Receiver *m_instance;

   static pcap_t *adhandle;
   static pcap_if_t *d;
   static QString IPaddrSrc;
   static BYTE IPaddrSrcByte[4];
   static BYTE localMAC[6];
   static QList<UDP_Receiver::ip_address> listIP;
   static QList<UDP_Receiver::mac_address> listMAC;
 //  static UDP_Receiver::ip_address destinationIP;
   int PacketDataLen;
   uchar *FinalPacket;
   static uchar FPS_cnt;
   uchar *pCurrentBuffer;


signals:
    int  onChooseAdapter();
    void onBufRdy(QString Read, QString ARUtype, unsigned char *buf, unsigned char *Temp);
    void onEcho(unsigned char *buf);
    void onMessageBox(QString type, QString txt);
    void onIP_MAC_receive(QList<ip_address> *ip, QList<mac_address> *mac);
    void onUpdateFrameCounter(unsigned char fc);
    void onPrintFlags(unsigned int);

public slots:
    void InitUDP_Receiver(QString despr);
    void ARPrequest(void);
    void TimerOut(void);
    void ChgIP(QString IPstr);


private slots:




};

#endif // UDP_RECEIVER_H

#include "UDP_Receiver.h"

pcap_t *UDP_Receiver::adhandle;
pcap_if_t *UDP_Receiver::d;
QString UDP_Receiver::IPaddrSrc;
BYTE UDP_Receiver::IPaddrSrcByte[4];
BYTE UDP_Receiver::localMAC[6];
static bool arp_find_flag;
QList<UDP_Receiver::ip_address> UDP_Receiver::listIP;
QList<UDP_Receiver::mac_address> UDP_Receiver::listMAC;
bool UDP_Receiver::FLAG_WRITE_READY;
uchar UDP_Receiver::FPS_cnt;
//UDP_Receiver::ip_address destinationIP;

UDP_Receiver::UDP_Receiver(QObject *parent) : QObject(parent)
{

}

UDP_Receiver::~UDP_Receiver()
{

}

void UDP_Receiver::InitUDP_Receiver(QString despr)
{
    timer = new QTimer();
    timer->setInterval(100);
    timer->stop();

    //enable writing data
    FLAG_WRITE_READY = true;

    pcap_if_t *alldevs;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE];
    u_int netmask;
    u_int mask;
    char  packet_filter[] =  "UDP";
    struct bpf_program fcode;
    //WinAPI
    PIP_ADAPTER_INFO pAdapterInfo;
    ULONG iAdapterInfo, bufAdapter;
    PIP_ADAPTER_ADDRESSES pAdapter;

    // Get the device list
    iAdapterInfo = 0;
    bufAdapter = 0;
    bufAdapter = WORKING_BUFFER_SIZE;
    int iErr = GetAdaptersInfo(NULL, &iAdapterInfo);

    if (iErr && (iErr != ERROR_BUFFER_OVERFLOW))
    {
        onMessageBox("crit", "Get adapters info failed");
    }
    if (!(pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, iAdapterInfo)))
    {
        onMessageBox("crit", "Memory allocating error");
    }
    pAdapter = (IP_ADAPTER_ADDRESSES*) MALLOC(bufAdapter);
    if (pAdapter == NULL)
    {
         onMessageBox("crit", "Memory allocating error");
    }
    if (GetAdaptersInfo(pAdapterInfo, &iAdapterInfo))
    {
        onMessageBox("crit", "GetAdaptersInfo failed");
    }

    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        onMessageBox("crit", "There is no adapters");
        qDebug() << stderr << " Error in findalldevs: " << errbuf;
        exit(1);

    }

    DWORD erradr;
    erradr = GetAdaptersAddresses(AF_INET, 0, NULL, pAdapter, &bufAdapter);
    if (erradr == ERROR_BUFFER_OVERFLOW)
    {
        FREE(pAdapter);
        pAdapter = NULL;
    }

    //print the list
    i = 0;
//    BYTE localMAC[pAdapter->PhysicalAddressLength];
    for (d = alldevs; d; d = d->next)
    {
        qDebug() << i << " " << d->description << "   ";
        i++;
        if (d->description == despr)
        {
            IPaddrSrc = pAdapterInfo->IpAddressList.IpAddress.String;
            for (int l = 0; l < pAdapter->PhysicalAddressLength; ++l)
            {
                localMAC[l] = pAdapter->PhysicalAddress[l];
            }

            break;
        }
        pAdapterInfo->Next;
        pAdapter->Next;
    }
    uchar lcnt = 0;
    uchar k  = 0;

    for (int l = 0; l < IPaddrSrc.length(); ++l)
    {
        IPaddrSrcByte[k] = 0;
        ++lcnt;
        if ((IPaddrSrc[l] == '.') || (l == (IPaddrSrc.length() - 1)))
        {
            if (l == (IPaddrSrc.length() - 1))
            {
                ++lcnt;
                ++l;
            }

            uint mult = 1;
            uchar bufChar = 0;
            for (int c = 1; c < lcnt; ++c)
            {
               bufChar += mult * IPaddrSrc.at(l - c).digitValue();
               mult *= 10;
            }
            IPaddrSrcByte[k] = bufChar;
            lcnt = 0;
            ++k;
        }
    }
    qDebug() << "IPaddr: " << IPaddrSrc;
    if (i == 0)
    {
       pcap_freealldevs(alldevs);
    }
    qDebug() <<"pcap init: "<< QThread::currentThreadId();

//   pcap_lookupnet(d->name, &netp, &maskp, errbuf);
//   ip_addr.S_un.S_addr = netp;
//   qDebug() << "PPP: "<< ip_addr.S_un.S_addr;

    //open adadpter
//   d = alldevs;
    if ((adhandle = pcap_open_live(d->name,
                                   65536,
                                   1,  // try to set 0
                                   1000,
                                   errbuf
                                   )) == NULL)
    {
        // вывод ошибки
        pcap_freealldevs(alldevs);
    }

    //check the link layer. support only ethernet for simplicity
    if (pcap_datalink(adhandle) != DLT_EN10MB)
    {
        // вывод ошибки
     //   pcap_freealldevs(alldevs);
    }

    if (d->addresses != NULL)
    {
        //get mask of the first address of interface
     //   pcap_lookupnet(alldevs->name, &netmask, &mask, errbuf);
        netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
    }
    else
        netmask =0xffffff;

    //compile the filter
    if (pcap_compile(adhandle, &fcode,packet_filter, 1, netmask) < 0 )
    {
        // вывод ошибки
    //    pcap_freealldevs(alldevs);
    }

    //set filter
    if( pcap_setfilter(adhandle, &fcode) < 0)
    {
        // вывод ошибки
//       pcap_freealldevs(alldevs);
    }

//   pcap_freealldevs(alldevs);

    //start capture
    pcap_loop(adhandle, 0, packet_handler, NULL);

//    ImageProcessing_BVR = new ImageProcessing();
//    pCurrentBuffer = ImageProcessing_BVR->GetInputBuffer();
    pCurrentBuffer = NULL;
}

void UDP_Receiver::ARPrequest()
{
    BYTE dstIP[HLEN_PLEN & 0xFF];
    BYTE dstMAC[6];

    for (int d = 0; d < 6; ++d)
    {
        dstMAC[d] = 0;
    }
    for(int  i= 0; i < 3; ++i)
        dstIP[i] = IPaddrSrcByte[i];

/*  dstIP[3] = 54;
    CreatePacket("ARP",&localMAC[0],&dstMAC[0],&IPaddrSrcByte[0],&dstIP[0],0,0,0,0);*/

    arp_find_flag = true;
    for(int i = 2; i < 255; ++i)
    {
        if(i == 2)
        {
            listIP.clear();
            listMAC.clear();
        }

        if(i != IPaddrSrcByte[3])
        {
            dstIP[3] = i;
            CreatePacket("ARP",&localMAC[0],&dstMAC[0],&IPaddrSrcByte[0],&dstIP[0],0,0,0,0);
        }
    }

    arp_find_flag = false;
    getInstance()->emitIP_MAC_recieve(&listIP, &listMAC);
}

void UDP_Receiver::TimerOut()
{
    qDebug() << "FPS " << FPS_cnt;
    FPS_cnt = 0;
   /* timer->stop();
    arp_find_flag = false;
    getInstance()->emitIP_MAC_recieve(&listIP, &listMAC);*/
}

void UDP_Receiver::ChgIP(QString IPstr)
{
    uchar lcnt = 0;
    uchar k  = 0;

    for(int l =0; l < IPstr.length(); ++l)
    {
        IPaddrSrcByte[k] = 0;
        ++lcnt;
        if ((IPstr[l] == '.') || (l == (IPstr.length() - 1)))
        {
            if (l == (IPstr.length() - 1))
            {
                ++lcnt;
                ++l;
            }
            uint mult = 1;
            uchar bufChar = 0;
            for (int c = 1; c < lcnt; ++c)
            {
               bufChar += mult * IPstr.at(l - c).digitValue();
               mult *= 10;
            }
            IPaddrSrcByte[k] = bufChar;
            lcnt = 0;
            ++k;
        }
    }
}

void UDP_Receiver::packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
    struct tm *ltime;
    char timestr[16];
    arp_header *arp;
    ip_header *ih;
    udp_header *uh;
    u_int ip_len;
    u_short sport,dport;
    time_t local_tv_sec;
 //   udp_header *p;
    static uint i;
    static bool ServiceFramePacketIsReceived;
    static unsigned char echoMas[10];
//    static unsigned char *ptrImage;
    static unsigned int FrameCounter;
    static unsigned char BufferNumber;
    static uchar *pCurrentImageBuffer, *pWriteFrameBuffer, *pBuffer;

    static u_int16_t PctNum;
    u_int16_t PctCur;

    //char *pdat;

    //    qDebug() <<"pcap loop: "<< QThread::currentThreadId();
     // convert the timestamp to readable format
    local_tv_sec = header->ts.tv_sec;
    ltime = localtime(&local_tv_sec);
    strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

    // print timestamp and length of the packet

    //   qDebug()<<i <<". time: "<<timestr <<"."<<header->ts.tv_usec<<" len: "<<header->len;

    //retrieve the position of the ip header
    ih = (ip_header *)(pkt_data + 14);                                          //length of ethernet header

    //retrieve the position of the udp header
    ip_len = (ih->ver_ihl & 0x0F) * 4;
    uh = (udp_header *)((u_char*)ih + ip_len);
    //retrieve the position of arp header
    arp = (arp_header *)(pkt_data);

    //convert from network byte order to host byte order
    sport =  uh->sport ;
    dport =  uh->dport;

    if (uh->datagram[PACKET_SIGNATURE_INDEX] == 0x00)
    {
        //First Packet
        if (uh->datagram[PACKET_FLAG_INDEX] == SERVICE_FRAME_PACKET_FLAG && header->len == SERVICE_FRAME_PACKET_SIZE)
        {
            // qDebug()<<i <<". time: "<<timestr <<"."<<header->ts.tv_usec<<" len: "<<header->len;
            pCurrentImageBuffer = ImageProcessing::GetInputBufferS();
            BufferNumber = ImageProcessing::GetInputBufferCounterS();
            PctNum = ((0xFF & (ih->identification)) << 8) + ((ih->identification) >> 8);
            ServiceFramePacketIsReceived = true;
            i = 0;
            FrameCounter++;
    //        qDebug() << "FrameCounter: " << FrameCounter << "; BufferNumber: " << BufferNumber << "; pCurrentImageBuffer: " << (uint)pCurrentImageBuffer;
            getInstance()->emitUpdateFrameCounter(FrameCounter);
        }
        //Video packets
        if (uh->datagram[PACKET_FLAG_INDEX] == FRAME_PACKET_FLAG && header->len == FRAME_PACKET_FULL_SIZE && i < FRAME_PACKET_NUMBER && ServiceFramePacketIsReceived)
        {
            PctCur = ((0xFF & (ih->identification)) << 8) + ((ih->identification) >> 8);
    //        if(((PctCur - PctNum) == (i + 1) ) || ( (0xFFFF + PctCur - PctNum) == i))
            {
                memcpy(pCurrentImageBuffer + i * QUADRUPLE_FRAME_WIDTH, uh->datagram + 6, QUADRUPLE_FRAME_WIDTH);
                ++i;
    //            qDebug() << "PctNum: " << PctNum << " PctCurrent: " << PctCur << " i:  "<< i;
            }
    //        else
    //        {
    ////            qDebug() << " Bad Frame " << " PctNum: " << PctNum << " PctCurrent: " << PctCur << " i: " << i;
    //            ServiceFramePacketIsReceived = false;
    //            i = 0;
    //        }
            if (i == FRAME_PACKET_NUMBER)
            {
                ServiceFramePacketIsReceived = false;
    //            qDebug() << " End of Frame: " << "FrameCounter: " << FrameCounter << " i: " << i << "; BufferNumber: " << BufferNumber;
    //            i = 0;
    //            ImageProcessing::SetIsAnyInputBufferFull();
                ImageProcessing::SetIsInputBufferFull(BufferNumber);
                ImageProcessing::SetFrameCounterS(FrameCounter);
//                if (Form_VideoFrame::GetVideoRecordingFlag())
                {
                    pBuffer = pCurrentImageBuffer;
                    uchar *pBufferEnd;
                    pBufferEnd = pBuffer + BUFFER_SIZE;
                    pWriteFrameBuffer = ImageProcessing::GetWriteFrameBuffer();
                    while (pBuffer++ < pBufferEnd)
                    {
                        *pWriteFrameBuffer++ = *pBuffer;
                        *pWriteFrameBuffer++ = *pBuffer;
                    }
//                    pWriteFrameBuffer = ImageProcessing::GetWriteFrameBuffer();
//                    cv::Mat frm = cv::Mat(FRAME_HEIGHT, DOUBLE_FRAME_WIDTH, CV_8UC1, pWriteFrameBuffer);
//                    cv::Mat frm = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1, pCurrentImageBuffer);
//                    writer.write(frm);
//                    Form_VideoFrame::GetVideoWriter().write(frm);
//                    Form_VideoFrame::IncCurrentVideoRecordingFrameCounter();
                }
            }
        }
    }

    //qDebug()<< "identification "<< ih->identification <<"flags_fo " << ih->flags_fo;
    //if( (ih->flags_fo == 0x0200)&&(ih->identification == 0x0406) )
    if( (arp_find_flag) && (arp->arp_cmd == 0x0200) && (arp->ftype == 0x0608))
    {
/*    qDebug() << "PCAP  IP: "<< arp->src_ip.byte1 <<"."<< arp->src_ip.byte2<<"."<< arp->src_ip.byte3 <<"."<< arp->src_ip.byte4
                <<  "MAC:" << arp->src_mac.byte1 <<":"<< arp->src_mac.byte2<<":"<< arp->src_mac.byte3
                <<":"<< arp->src_mac.byte4<<":"<< arp->src_mac.byte5<<":"<< arp->src_mac.byte6;
*/
        listIP.append(arp->src_ip);
        listMAC.append(arp->src_mac);
    }
    else if(uh->datagram[1] == 0x03) // command echo
    {
        memcpy(&echoMas[0], uh->datagram, 10);
        getInstance()->emitEchoRdy(&echoMas[0]);
    }
}// function end

UDP_Receiver* UDP_Receiver::getInstance()
{
   static UDP_Receiver *m_instance = new UDP_Receiver();

    if (m_instance == 0)
        m_instance = new UDP_Receiver();
    return m_instance;
}

void UDP_Receiver::CreatePacket(QString Pckt_type,
                               BYTE *SourceMAC,
                               BYTE *DestinationMAC,
                               BYTE *SourceIP,
                               BYTE *DestIP,
                               unsigned short SourcePort,
                               unsigned short DestinationPort,
                               unsigned char UserData,
                               unsigned int UserDataLen)
{
    if (Pckt_type == "UDP")
    {
        UDP_Receiver::PacketDataLen = UserDataLen;
        // DataLength + 42 Bytes of Headers
        FinalPacket = new unsigned char[UserDataLen + 42];
        uint Pcktcnt = 0;
        uint PTR;
        memcpy((void*)FinalPacket, (void*)DestinationMAC, 6);                   // destMAC;
        Pcktcnt += 6;
        memcpy((void *)(FinalPacket + Pcktcnt), (void *)SourceMAC, 6);          // SrcMAC
        Pcktcnt += 6;
        PTR = IP_TYPE;
        memcpy((void *)(FinalPacket + Pcktcnt), (void *)&PTR, 2);
        Pcktcnt += 2;
    }
    else
        if(Pckt_type == "ARP")
        {
            FinalPacket = new unsigned char[60];
            uint PTR;
            uint Pcktcnt = 0;

            BYTE dstMACff[6];

            for(int d = 0; d  <6; ++d)
            {
                dstMACff[d] = 0xFF;
            }

            memcpy((void *)(FinalPacket), (void *)&dstMACff, 6);                //SrcMAC
            Pcktcnt += 6;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)SourceMAC, 6);      //SrcMAC
            Pcktcnt += 6;
            PTR = FTYPE;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)&PTR, 2);           //Frame type (FTYPE) Ethernet II
            Pcktcnt += 2;
            PTR = HTYPE;
//            memcpy((void *)FinalPacket + Pcktcnt, (void *)&PTR, 2);             //Hardware type (HTYPE)
//            Pcktcnt += 2;
//            PTR = PTYPE;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)&PTR, 2);           //Ptotocol type (PTYPE)
            Pcktcnt += 2;
            PTR = HLEN_PLEN;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)&PTR, 2);           //MAClen_IP_len
            Pcktcnt += 2;
            PTR = ARP_REQUEST;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)&PTR, 2);           //ARP request
            Pcktcnt += 2;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)SourceMAC, 6);      //SrcMAC
            Pcktcnt += 6;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)SourceIP, 4);       //SrcIP
            Pcktcnt += 4;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)DestinationMAC, 6 );//DestMAC
            Pcktcnt += 6;
            memcpy((void *)(FinalPacket + Pcktcnt), (void *)DestIP, 4);         //DestIP
            Pcktcnt += 4;

            for(int s = 0; s < (60 - 42); ++s )
            {
                uchar b = 0x00;
                memcpy((void*)(FinalPacket + Pcktcnt), (void *)&b, 1);
                Pcktcnt += 1;
            }
            pcap_sendpacket(adhandle, FinalPacket, Pcktcnt);
        }
}

void UDP_Receiver::SendPcktPcapUdp(QByteArray Data, int len)
{

}

void UDP_Receiver::emitBufRdy(unsigned char *buf, unsigned char *Temp)
{
    emit onBufRdy("R", "R", buf, Temp);
  //  emit onHistRdy(buf);
}

void UDP_Receiver::emitIP_MAC_recieve(QList<ip_address> *ip, QList<mac_address> *mac)
{
    emit onIP_MAC_receive(ip, mac);
}

void UDP_Receiver::emitEchoRdy(unsigned char *buf)
{
    emit onEcho(buf);
}


void UDP_Receiver::emitUpdateFrameCounter(unsigned char fc)
{
    emit onUpdateFrameCounter(fc);
}






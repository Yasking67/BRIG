#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QObject>

#include "Converts.h"

#define TEST_MODE 0
#define OPERATING_MODE 1
#define CONFIG_MODE TEST_MODE

#define RECEIVING_DATA_TYPE_NUMBER 1

#define CONTROL_UDP_PREAMBLE                                0x0000

#define RECEIVING_PREAMBLE_COMMAND                          0x50
#define RECEIVING_PREAMBLE_TEST                             0x55
#define SENDING_PREAMBLE_0                                  0x52
#define SENDING_PREAMBLE_1                                  0x55

union MainFlags
{
    struct
    {
        unsigned	Mode                                    :2;                 //
    };
//    unsigned int all;
      quint64 all;
};

union ReceivingNewDataFlags
{
    struct
    {
        unsigned    AnyReceiving                                                :RECEIVING_DATA_TYPE_NUMBER;
    };
    struct
    {
        unsigned    SettingsReceiving                                           :1;
    };
    uint all;
};

class DataProcessing
{

public:
    DataProcessing();

    static ReceivingNewDataFlags ReceivingNewDataFlagsDP;
    static void SetNewSettingsValues(uint);

    static MainFlags GetMainFlags(void) {return MainFlagsDP;}
    static quint64 GetMainFlagsAll(void) {return MainFlagsDP.all;}

    static uint GetCCP_IP_Address(void) {return CCP_IP_Address;}
    static uint GetDeviceIP_Address(void) {return DeviceIP_Address;}
    static ushort GetCCP_Port(void) {return CCP_Port;}
    static ushort GetDevicePort(void) {return DevicePort;}
private:
    static MainFlags MainFlagsDP;
    static uint StartPulseCounter;

    static uint CCP_IP_Address, DeviceIP_Address;
    static ushort CCP_Port, DevicePort;
};

#endif // DATAPROCESSING_H

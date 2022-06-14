#include "DataProcessing.h"

MainFlags DataProcessing::MainFlagsDP;
ReceivingNewDataFlags DataProcessing::ReceivingNewDataFlagsDP;
uint DataProcessing::CCP_IP_Address, DataProcessing::DeviceIP_Address;
ushort DataProcessing::CCP_Port, DataProcessing::DevicePort;

DataProcessing::DataProcessing()
{
    ReceivingNewDataFlagsDP.AnyReceiving = 0;

    if (CONFIG_MODE == TEST_MODE)
    {
        CCP_IP_Address = (169 << 24) + (254 << 16) + (0 << 8) + 1;
        DeviceIP_Address = (169 << 24) + (254 << 16) + (0 << 8) + 13;
        CCP_Port = 52515;
        DevicePort = 52516;
    }
    else
    {
        CCP_IP_Address = (192 << 24) + (168 << 16) + (1 << 8) + 10;
        DeviceIP_Address = (192 << 24) + (168 << 16) + (1 << 8) + 225;
        CCP_Port = 52516;
        DevicePort = 52516;
    }
}

void DataProcessing::SetNewSettingsValues(uint f)
{
    MainFlagsDP.all = f;
    ReceivingNewDataFlagsDP.SettingsReceiving = 1;
}

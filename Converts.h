#ifndef CONVERTS_H
#define CONVERTS_H

#include <QObject>
//#include <QStringRef>

class Converts
{
public:
    Converts();

    typedef struct
    {
        union
        {
            struct
            {
                unsigned char byte0;
                unsigned char byte1;
                unsigned char byte2;
                unsigned char byte3;
            };
            unsigned int uint_32;
            signed int int_32;
            float float_32;
        };
    } float_data_32;

    QByteArray StringToHexByteArray(QString);
    static QString IntToBinStr(uint , uchar , uchar);
    static uint RoundToUint(double);
    static int RoundToInt(double);
    static uint FloatToUintPack(float);
    static float FloatFromUintUnpack(uint);
    static QByteArray IP_AddressStringToQByteArray(QString);
    static uchar *IP_AddressStringToUcharArray(QString);
};

#endif // CONVERTS_H

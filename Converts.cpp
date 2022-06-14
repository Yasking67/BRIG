#include "Converts.h"

Converts::Converts()
{

}

QByteArray Converts::StringToHexByteArray(QString str)
{
    QByteArray array;



    return array;
}

QString Converts::IntToBinStr(uint value_int, uchar bit_number, uchar space_position)
{
    QString str;
    uchar i;
    str = QString::number(value_int, 2);
    if (str.length() < bit_number)
    {
        for (i = str.length(); i < bit_number; i++)
            str = str.prepend('0');
        for (i = 0; i < (bit_number - 1) / space_position; i++)
        {
            str = str.insert(space_position * ((bit_number % space_position != 0) ? i : i + 1) + i + bit_number % space_position, " ");
        }
    }
    else
        str.remove(0, str.length() - bit_number);
    return str;
}

uint Converts::RoundToUint(double value)
{
    return value - (uint)value > 0.5 ? (uint)value + 1 : (uint)value;
}

int Converts::RoundToInt(double value)
{
    if (value > 0)
        return value - (int)value > 0.5 ? (int)value + 1 : (int)value;
    else
        return value - (int)value > -0.5 ? (int)value : (int)value - 1;
}

uint Converts::FloatToUintPack(float value)
{
    float_data_32 temp;
    temp.float_32 = value;
    return temp.uint_32;
}

float Converts::FloatFromUintUnpack(uint value)
{
    float_data_32 temp;
    temp.uint_32 = value;
    return temp.float_32;
}

QByteArray Converts::IP_AddressStringToQByteArray(QString str)
{
    QByteArray array;
    array.resize(4);
    int i, index = 0, ByteCounter = 0;
    for (i = 0; i < str.length(); i++)
    {
        if (str[i] == '.')
        {
            QStringRef subString(&str, index, i - index);
            array[ByteCounter++] = subString.toUShort();
            index = i + 1;
        }
        if (ByteCounter == 4)
            break;
    }
    if (ByteCounter < 4)
    {
        QStringRef subString(&str, index, i - index);
        array[ByteCounter++] = subString.toUShort();
    }
    return array;
}

uchar *Converts::IP_AddressStringToUcharArray(QString str)
{
    uchar array[4];
    int i, index = 0, ByteCounter = 0;
    for (i = 0; i < str.length(); i++)
    {
        if (str[i] == '.')
        {
            QStringRef subString(&str, index, i - index);
            array[ByteCounter++] = subString.toUShort();
            index = i + 1;
        }
        if (ByteCounter == 4)
            break;
    }
    if (ByteCounter < 4)
    {
        QStringRef subString(&str, index, i - index);
        array[ByteCounter++] = subString.toUShort();
    }
    return array;
}

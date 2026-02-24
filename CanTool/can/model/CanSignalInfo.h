#ifndef CANSIGNALINFO_H
#define CANSIGNALINFO_H

#include <QString>
#include <cstdint>

struct CanSignalInfo
{
    QString name;

    uint16_t startBit = 0;
    uint16_t bitLength = 0;

    bool isLittleEndian = true;
    bool isSigned = false;

    double scale  = 1.0;
    double offset = 0.0;

    double minValue = 0.0;
    double maxValue = 0.0;

    QString unit;
};


#endif // CANSIGNALINFO_H

#ifndef CANMESSAGEINFO_H
#define CANMESSAGEINFO_H

#include <QString>
#include <QVector>
#include <cstdint>

#include "CanSignalInfo.h"

struct CanMessageInfo
{
    uint32_t id = 0;
    QString  name;
    uint8_t  dlc = 0;

    QVector<CanSignalInfo> signalList;
};

#endif // CANMESSAGEINFO_H

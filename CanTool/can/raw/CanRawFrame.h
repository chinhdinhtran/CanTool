#ifndef CANRAWFRAME_H
#define CANRAWFRAME_H

#include <cstdint>
#include <QtGlobal>

struct CanRawFrame
{
    uint32_t id = 0;
    uint8_t  dlc   = 0;
    uint8_t  data[64]{};
    qint64   timestamp = 0;
    bool     isExtended = false;
    uint8_t  channel = 0;
    bool     isFd = false;
};

#endif // CANRAWFRAME_H

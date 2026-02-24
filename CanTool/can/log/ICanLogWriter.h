#ifndef ICANLOGWRITER_H
#define ICANLOGWRITER_H


#include <QObject>
#include "can/raw/CanRawFrame.h"

class ICanLogWriter
{
public:
    virtual ~ICanLogWriter() = default;

    virtual void startLogging() = 0;
    virtual void stopLogging() = 0;
    virtual void onFrame(const CanRawFrame& frame) = 0;
};


#endif // ICANLOGWRITER_H

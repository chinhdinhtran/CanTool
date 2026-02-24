#ifndef ICANDRIVER_H
#define ICANDRIVER_H


#include "can/raw/CanRawFrame.h"

/**
 * @brief ICanDriver
 * Abstract CAN driver interface
 */
class ICanDriver
{
public:
    virtual ~ICanDriver() = default;

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual bool send(const CanRawFrame& frame) = 0;
    virtual bool receive(CanRawFrame& frame) = 0;
};

#endif // ICANDRIVER_H

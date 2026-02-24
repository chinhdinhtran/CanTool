#pragma once

#include "ICanDriver.h"

#include <windows.h>
#include "vxlapi.h"

/**
 * @brief VectorCanDriver
 * Implementation of ICanDriver using Vector XL API
 */
class VectorCanDriver : public ICanDriver
{
public:
    VectorCanDriver();
    ~VectorCanDriver() override;

    bool open() override;
    void close() override;

    bool send(const CanRawFrame& frame) override;
    bool receive(CanRawFrame& frame) override;

private:
    XLportHandle m_portHandle = XL_INVALID_PORTHANDLE;
    XLaccess     m_channelMask = 0;
};

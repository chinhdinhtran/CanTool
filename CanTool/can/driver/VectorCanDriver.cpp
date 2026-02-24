#include "VectorCanDriver.h"

#include <QDebug>
#include <cstring>

VectorCanDriver::VectorCanDriver() {}

VectorCanDriver::~VectorCanDriver()
{
    close();
}

bool VectorCanDriver::open()
{
    XLstatus status;

    status = xlOpenDriver();
    if (status != XL_SUCCESS)
    {
        qDebug() << "xlOpenDriver failed";
        return false;
    }

    XLdriverConfig cfg;
    status = xlGetDriverConfig(&cfg);
    if (status != XL_SUCCESS || cfg.channelCount == 0)
    {
        qDebug() << "No Vector device found";
        return false;
    }

    unsigned int hwType = 0, hwIndex = 0, hwChannel = 0;
    bool found = false;

    for (unsigned int i = 0; i < cfg.channelCount; ++i)
    {
        auto& ch = cfg.channel[i];
        if ((ch.channelBusCapabilities & XL_BUS_ACTIVE_CAP_CAN) &&
            ch.busParams.busType == XL_BUS_TYPE_CAN)
        {
            hwType = ch.hwType;
            hwIndex = ch.hwIndex;
            hwChannel = ch.hwChannel;
            found = true;
            break;
        }
    }

    if (!found)
    {
        qDebug() << "No CAN channel found";
        return false;
    }

    m_channelMask = xlGetChannelMask(hwType, hwIndex, hwChannel);

    char appName[] = "Qt_CAN_Tool_By_PWT_Team";
    status = xlSetApplConfig(appName, 0,
                             hwType, hwIndex, hwChannel,
                             XL_BUS_TYPE_CAN);
    if (status != XL_SUCCESS)
    {
        qDebug() << "xlSetApplConfig failed";
        return false;
    }

    XLaccess permissionMask = m_channelMask;
    status = xlOpenPort(&m_portHandle,
                        appName,
                        m_channelMask,
                        &permissionMask,
                        1024,
                        XL_INTERFACE_VERSION,
                        XL_BUS_TYPE_CAN);
    if (status != XL_SUCCESS || permissionMask == 0)
    {
        qDebug() << "xlOpenPort failed";
        return false;
    }

    status = xlCanSetChannelBitrate(m_portHandle, m_channelMask, 500000);
    if (status != XL_SUCCESS)
    {
        qDebug() << "Set bitrate failed";
        return false;
    }

    xlCanSetChannelMode(m_portHandle, m_channelMask, 0, 0);

    status = xlActivateChannel(m_portHandle,
                               m_channelMask,
                               XL_BUS_TYPE_CAN,
                               XL_ACTIVATE_RESET_CLOCK);
    if (status != XL_SUCCESS)
    {
        qDebug() << "Activate channel failed";
        return false;
    }

    qDebug() << "Vector CAN opened successfully";
    return true;
}

void VectorCanDriver::close()
{
    qDebug() << Q_FUNC_INFO;
    if (m_portHandle != XL_INVALID_PORTHANDLE)
    {
        xlDeactivateChannel(m_portHandle, m_channelMask);
        xlClosePort(m_portHandle);
        xlCloseDriver();
        m_portHandle = XL_INVALID_PORTHANDLE;
        m_channelMask = 0;
    }
}

bool VectorCanDriver::send(const CanRawFrame& frame)
{
    if (m_portHandle == XL_INVALID_PORTHANDLE)
        return false;

    XLevent ev{};
    ev.tag = XL_TRANSMIT_MSG;
    ev.tagData.msg.id = frame.id;
    ev.tagData.msg.dlc = frame.dlc;
    ev.tagData.msg.flags = frame.isExtended ? XL_CAN_EXT_MSG_ID : 0;
    std::memcpy(ev.tagData.msg.data, frame.data, 8);

    unsigned int cnt = 1;
    return xlCanTransmit(m_portHandle,
                         m_channelMask,
                         &cnt,
                         &ev) == XL_SUCCESS;
}

bool VectorCanDriver::receive(CanRawFrame& frame)
{
    if (m_portHandle == XL_INVALID_PORTHANDLE)
        return false;

    XLevent ev{};
    unsigned int cnt = 1;

    if (xlReceive(m_portHandle, &cnt, &ev) != XL_SUCCESS || cnt == 0)
        return false;

    if (ev.tag != XL_CAN_EV_TAG_RX_OK)
        return false;

    auto& msg = ev.tagData.msg;

    frame.id = msg.id;
    frame.dlc = msg.dlc;
    std::memcpy(frame.data, msg.data, 8);
    frame.isExtended = (msg.flags & XL_CAN_EXT_MSG_ID) != 0;
    frame.timestamp = ev.timeStamp;

    return true;
}

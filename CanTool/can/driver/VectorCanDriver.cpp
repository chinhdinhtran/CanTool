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

    char appName[] = "Qt_CAN_Tool_By_PWTTM_Team";

    m_channelMask = 0;
    int appChannel = 0;

    QVector<XLaccess> channelMasks;

    for (unsigned int i = 0; i < cfg.channelCount; ++i)
    {
        auto& ch = cfg.channel[i];

        if ((ch.channelBusCapabilities & XL_BUS_ACTIVE_CAP_CAN) &&
            ch.busParams.busType == XL_BUS_TYPE_CAN)
        {
            XLaccess mask = xlGetChannelMask(
                ch.hwType,
                ch.hwIndex,
                ch.hwChannel
            );

            m_channelMask |= mask;
            channelMasks.push_back(mask);

            XLstatus s = xlSetApplConfig(appName,
                                         appChannel++,
                                         ch.hwType,
                                         ch.hwIndex,
                                         ch.hwChannel,
                                         XL_BUS_TYPE_CAN);

            if (s != XL_SUCCESS)
            {
                qDebug() << "xlSetApplConfig failed for channel" << i;
                return false;
            }
        }
    }

    if (m_channelMask == 0)
    {
        qDebug() << "No CAN channels found";
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

    status = xlSetNotification(m_portHandle, &m_eventHandle, 1);
    if (status != XL_SUCCESS)
    {
        qDebug() << "xlSetNotification failed";
        return false;
    }

    for (auto mask : channelMasks)
    {
        status = xlCanSetChannelBitrate(m_portHandle, mask, 500000);
        if (status != XL_SUCCESS)
        {
            qDebug() << "Set bitrate failed";
            return false;
        }
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

    qDebug() << "Vector CAN multi-channel opened successfully";
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
        m_eventHandle = nullptr;
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

    DWORD rc = WaitForSingleObject(m_eventHandle, 100);

    if (rc != WAIT_OBJECT_0)
        return false;

    XLevent ev{};
    unsigned int cnt = 1;

    XLstatus status = xlReceive(m_portHandle, &cnt, &ev);

    if (status != XL_SUCCESS || cnt == 0)
        return false;

    if (ev.tag != XL_RECEIVE_MSG)
        return false;

    const auto& msg = ev.tagData.msg;

    frame.id = msg.id;
    frame.dlc = msg.dlc;
    frame.isFd = (msg.flags & XL_CAN_RXMSG_FLAG_EDL) != 0;

    uint8_t copyLen = std::min<uint8_t>(msg.dlc, sizeof(frame.data));
    std::memcpy(frame.data, msg.data, copyLen);

    frame.isExtended = (msg.flags & XL_CAN_EXT_MSG_ID) != 0;
    frame.channel = ev.chanIndex;
    frame.timestamp = ev.timeStamp;

    return true;
}
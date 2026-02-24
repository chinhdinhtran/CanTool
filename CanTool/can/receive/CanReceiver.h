#pragma once

#include <QThread>

#include "can/driver/ICanDriver.h"
#include "can/dbc/DbcManager.h"
#include "can/decode/DecodedSignalValue.h"

/**
 * @brief CanReceiver
 * Dedicated RX thread: receive raw CAN frames, decode them
 */
class CanReceiver : public QThread
{
    Q_OBJECT
public:
    CanReceiver(ICanDriver* driver,
                DbcManager* dbc,
                QObject* parent = nullptr);

Q_SIGNALS:
    void decodedSignal(const DecodedSignalValue& value);
    void rawFrameReceived(const CanRawFrame& frame);

protected:
    void run() override;

private:
    ICanDriver* m_driver;
    DbcManager* m_dbc;
};

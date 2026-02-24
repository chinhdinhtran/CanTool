#include "CanReceiver.h"

#include "utils/TimeUtils.h"

CanReceiver::CanReceiver(ICanDriver* driver,
                         DbcManager* dbc,
                         QObject* parent)
    : QThread(parent),
    m_driver(driver),
    m_dbc(dbc)
{
}

void CanReceiver::run()
{
    // //DUMMY BLF Writer
    // uint64_t ts = 0;

    // const int FRAMES_PER_BURST = 50;
    // const uint64_t STEP_NS = 50'000ULL;
    // const int BURST_SLEEP_MS = 1;

    // while (!isInterruptionRequested())
    // {
    //     // ===== BURST FRAME =====
    //     for (int n = 0; n < FRAMES_PER_BURST; ++n)
    //     {
    //         CanRawFrame frame{};
    //         frame.id = 0x100 + (ts / STEP_NS) % 32;
    //         frame.dlc = 8;
    //         frame.isExtended = false;

    //         for (int i = 0; i < 8; ++i)
    //             frame.data[i] = static_cast<uint8_t>((n + i) & 0xFF);

    //         frame.timestamp = ts;
    //         ts += STEP_NS;

    //         emit rawFrameReceived(frame);
    //     }

    //     QThread::msleep(BURST_SLEEP_MS);
    // }




    while (!isInterruptionRequested())
    {
        CanRawFrame frame;
        if (!m_driver->receive(frame))
            continue;
        emit rawFrameReceived(frame);
        QVector<DecodedSignalValue> decodedSignals;
        m_dbc->decodeFrame(frame, decodedSignals);

        for (const auto& sig : decodedSignals)
            emit decodedSignal(sig);
    }
}

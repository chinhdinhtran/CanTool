#include <QDebug>
#include "DbcManager.h"
#include "can/dbc/DbcLibWrapper.h"
#include <can/model/CanSignalInfo.h>

bool DbcManager::loadFile(const QString& dbcPath)
{
    // m_canInfo.clear();  //do not clear the model, support mutil .dbc file

    m_dbcFile = std::make_shared<dbc::DbcFile>();
    m_dbcFile->Filename(dbcPath.toStdString());

    if (!m_dbcFile->ParseFile())
    {
        qCritical() << "DBC parse failed:"
                    << QString::fromStdString(m_dbcFile->LastError());
        m_dbcFile.reset();
        return false;
    }
    parse();
    qDebug() << "DBC parse OK.";
    return true;
}

void DbcManager::clearModel()
{
    qDebug() << Q_FUNC_INFO;
    m_canInfo.clear();
    m_dbcFile.reset();
}

const CanInformation& DbcManager::canInformation() const
{
    return m_canInfo;
}

void DbcManager::parse()
{
    if (!m_dbcFile)
        return;

    dbc::Network* network = m_dbcFile->GetNetwork();
    if (!network)
        return;

    for (const auto& msgPair : network->Messages())
    {
        const dbc::Message& msg = msgPair.second;

        CanMessageInfo msgInfo;
        msgInfo.id   = static_cast<uint32_t>(msg.CanId());
        msgInfo.name = QString::fromStdString(msg.Name());
        msgInfo.dlc  = static_cast<uint8_t>(msg.NofBytes());

        for (const auto& sigPair : msg.Signals())
        {
            const dbc::Signal& sig = sigPair.second;

            CanSignalInfo s;
            s.name = QString::fromStdString(sig.Name());

            s.startBit  = static_cast<uint16_t>(sig.BitStart());
            s.bitLength = static_cast<uint16_t>(sig.BitLength());

            s.isLittleEndian = sig.LittleEndian();

            s.isSigned = (sig.DataType() == dbc::SignalDataType::SignedData);

            s.scale  = sig.Scale();
            s.offset = sig.Offset();

            s.minValue = sig.Min();
            s.maxValue = sig.Max();

            s.unit = QString::fromStdString(sig.Unit());

            msgInfo.signalList.push_back(s);
        }

        m_canInfo.addMessage(msgInfo);
    }

    qDebug() << "DBC parsed OK. Messages:" << m_canInfo.messages().size();


/* print log
    for (auto it = m_canInfo.messages().begin();
         it != m_canInfo.messages().end(); ++it)
    {
        const CanMessageInfo& msg = it.value();

        qDebug() << "Message:"
                 << QString("0x%1").arg(msg.id, 0, 16)
                 << msg.name
                 << "DLC:" << msg.dlc;

        for (const auto& sig : msg.signalList)
        {
            qDebug() << "  Signal:"
                     << sig.name
                     << "Start:" << sig.startBit
                     << "Len:" << sig.bitLength
                     << "Scale:" << sig.scale
                     << "Offset:" << sig.offset
                     << "Unit:" << sig.unit;
        }
    }
*/
}

dbc::Network* DbcManager::network() const
{
    return m_dbcFile ? m_dbcFile->GetNetwork() : nullptr;
}

static bool extractBitsSafe(
    const uint8_t* data,
    uint16_t dlc,
    uint16_t startBit,
    uint16_t bitLength,
    bool littleEndian,
    uint64_t& out)
{
    out = 0;
    uint16_t maxBits = dlc * 8;

    if (bitLength == 0 || bitLength > 64)
        return false;

    if (littleEndian) {
        if (startBit + bitLength > maxBits)
            return false;

        for (uint16_t i = 0; i < bitLength; ++i) {
            uint16_t bitIndex = startBit + i;
            uint8_t byte = data[bitIndex / 8];
            uint8_t bit  = (byte >> (bitIndex % 8)) & 1;
            out |= (uint64_t(bit) << i);
        }
    }
    else {
        if (startBit + 1 < bitLength)
            return false;

        for (uint16_t i = 0; i < bitLength; ++i) {
            int bitIndex = startBit - i;
            if (bitIndex < 0 || bitIndex >= maxBits)
                return false;

            uint8_t byte = data[bitIndex / 8];
            uint8_t bit  = (byte >> (7 - (bitIndex % 8))) & 1;
            out = (out << 1) | bit;
        }
    }
    return true;
}

static int64_t signExtend(uint64_t value, uint16_t bitLength)
{
    uint64_t mask = 1ULL << (bitLength - 1);
    return (value ^ mask) - mask;
}

void DbcManager::decodeFrame(const CanRawFrame& frame, QVector<DecodedSignalValue>& out)
{
    if (!m_canInfo.hasMessage(frame.id))
    {
        return;
    }
    const CanMessageInfo& msgInfo = m_canInfo.message(frame.id);
    if (frame.dlc < msgInfo.dlc)
        return;
    out.reserve(msgInfo.signalList.size());
    for (const CanSignalInfo& sig : msgInfo.signalList)
    {
        uint64_t raw;
        if (!extractBitsSafe(
                frame.data,
                frame.dlc,
                sig.startBit,
                sig.bitLength,
                sig.isLittleEndian,
                raw))
        {
            continue;
        }
        int64_t signedRaw = sig.isSigned
            ? signExtend(raw, sig.bitLength)
            : static_cast<int64_t>(raw);
        double physical =
            signedRaw * sig.scale + sig.offset;
        if (physical < sig.minValue || physical > sig.maxValue)
            continue;
        double ts_ms = static_cast<double>(frame.timestamp) / 1e6;
        out.push_back({
            sig.name,
            physical,
            static_cast<double>(ts_ms)
        });
    }
}
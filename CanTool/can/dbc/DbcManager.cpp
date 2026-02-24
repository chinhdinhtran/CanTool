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

void DbcManager::decodeFrame(const CanRawFrame& frame, QVector<DecodedSignalValue>& out)
{
    if (!m_canInfo.hasMessage(frame.id))
        return;

    if (!m_dbcFile)
        return;

    dbc::Network* network = m_dbcFile->GetNetwork();
    if (!network)
        return;

    const auto& msgInfo = m_canInfo.message(frame.id);

    dbc::Message* dbcMsg = network->GetMessageByCanId(frame.id);
    if (!dbcMsg)
        return;

    std::vector<uint8_t> data(frame.data, frame.data + frame.dlc);

    out.reserve(msgInfo.signalList.size());

    for (const auto& sigInfo : msgInfo.signalList)
    {
        dbc::Signal* sig =
            dbcMsg->GetSignal(sigInfo.name.toStdString());
        if (!sig)
            continue;

        sig->ParseMessage(data, frame.timestamp, frame.id);

        double v;
        if (!sig->EngValue(v))
            continue;

        out.push_back({
            sigInfo.name,
            v,
            static_cast<double>(frame.timestamp)
        });
    }
}


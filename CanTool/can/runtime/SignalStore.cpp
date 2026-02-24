#include "SignalStore.h"

SignalStore::SignalStore(QObject* parent)
    : QObject(parent)
{
}

SignalBuffer* SignalStore::buffer(const QString& signalId)
{
    auto it = m_buffers.find(signalId);
    if (it == m_buffers.end())
    {
        auto buf = std::make_unique<SignalBuffer>();
        SignalBuffer* ptr = buf.get();
        m_buffers.emplace(signalId, std::move(buf));
        return ptr;
    }
    return it->second.get();
}

void SignalStore::appendSample(const QString& signalId,
                               double timestamp,
                               double value)
{
    buffer(signalId)->append(timestamp, value);
    // emit signalUpdated(signalId);
}

void SignalStore::append(const DecodedSignalValue& value)
{
    appendSample(value.signalId,
                 value.timestamp,
                 value.physicalValue);
}

void SignalStore::clear()
{
    m_buffers.clear();
}

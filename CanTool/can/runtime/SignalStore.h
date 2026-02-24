#pragma once

#include <QObject>
#include <unordered_map>
#include <memory>
#include <QString>

#include "SignalBuffer.h"
#include "can/decode/DecodedSignalValue.h"

class SignalStore : public QObject
{
    Q_OBJECT
public:
    explicit SignalStore(QObject* parent = nullptr);

    SignalBuffer* buffer(const QString& signalId);

    void appendSample(const QString& signalId,
                      double timestamp,
                      double value);

    void append(const DecodedSignalValue& value);
    void clear();

Q_SIGNALS:
    void signalUpdated(const QString& signalId);

private:
    std::unordered_map<QString, std::unique_ptr<SignalBuffer>> m_buffers;
};

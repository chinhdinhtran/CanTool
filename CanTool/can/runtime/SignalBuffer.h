#pragma once

#include <QVector>
#include "SignalSample.h"

/**
 * @brief SignalBuffer
 * Stores time-series samples of ONE signal
 */
class SignalBuffer
{
public:
    explicit SignalBuffer(int capacity = 5000);

    void append(double timestamp, double value);
    const QVector<SignalSample>& data() const;

    void clear();
    int size() const;

private:
    QVector<SignalSample> m_buffer;
    int m_capacity = 0;
};

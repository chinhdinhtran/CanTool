#include "SignalBuffer.h"

SignalBuffer::SignalBuffer(int capacity)
    : m_capacity(capacity)
{
    m_buffer.reserve(m_capacity);
}

void SignalBuffer::append(double timestamp, double value)
{
    if (m_buffer.size() >= m_capacity)
    {
        m_buffer.removeFirst();
    }
    m_buffer.push_back({ timestamp, value });
}

const QVector<SignalSample>& SignalBuffer::data() const
{
    return m_buffer;
}

void SignalBuffer::clear()
{
    m_buffer.clear();
}

int SignalBuffer::size() const
{
    return m_buffer.size();
}

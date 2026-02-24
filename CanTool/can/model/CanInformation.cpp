#include "CanInformation.h"

void CanInformation::clear()
{
    m_messages.clear();
}

void CanInformation::addMessage(const CanMessageInfo& msg)
{
    m_messages.insert(msg.id, msg);
}

bool CanInformation::hasMessage(uint32_t id) const
{
    return m_messages.contains(id);
}

const CanMessageInfo& CanInformation::message(uint32_t id) const
{
    return m_messages.value(id);
}

const QMap<uint32_t, CanMessageInfo>&
CanInformation::messages() const
{
    return m_messages;
}

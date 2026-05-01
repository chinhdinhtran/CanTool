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

//fix crash
const CanMessageInfo& CanInformation::message(uint32_t id) const
{
    auto it = m_messages.constFind(id);
    Q_ASSERT(it != m_messages.constEnd());
    return it.value();
}

const QMap<uint32_t, CanMessageInfo>&
CanInformation::messages() const
{
    return m_messages;
}

#ifndef CANINFORMATION_H
#define CANINFORMATION_H

#include <QMap>
#include <cstdint>

#include "CanMessageInfo.h"

class CanInformation
{
public:
    void clear();

    void addMessage(const CanMessageInfo& msg);

    bool hasMessage(uint32_t id) const;
    const CanMessageInfo& message(uint32_t id) const;

    const QMap<uint32_t, CanMessageInfo>& messages() const;

private:
    QMap<uint32_t, CanMessageInfo> m_messages;
};

#endif // CANINFORMATION_H

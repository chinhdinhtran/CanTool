#pragma once

#include "SignalMeta.h"
#include "../model/CanSignalInfo.h"

class SignalClassifier
{
public:
    static SignalMeta classify(const CanSignalInfo& signalInfo,
                               const QString& signalId);
};

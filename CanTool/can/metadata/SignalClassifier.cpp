#include "SignalClassifier.h"

SignalMeta SignalClassifier::classify(const CanSignalInfo& signalInfo,
                                      const QString& signalId)
{
    SignalMeta meta;
    meta.signalId = signalId;
    meta.displayName = signalInfo.name;
    meta.unit = signalInfo.unit;

    if (signalInfo.bitLength == 1) {
        meta.type = SignalType::Digital;
        meta.recommendedPlot = PlotRecommendation::DigitalState;
        return meta;
    }

    bool isDiscreteLike = !signalInfo.isSigned &&
                          signalInfo.scale == 1.0 &&
                          signalInfo.offset == 0.0 &&
                          (signalInfo.maxValue - signalInfo.minValue) <= 1023.0 &&
                          signalInfo.bitLength <= 16;

    QString nameLower = signalInfo.name.toLower();
    bool hasStateKeyword = nameLower.contains("mode") ||
                           nameLower.contains("status") ||
                           nameLower.contains("gear") ||
                           nameLower.contains("door") ||
                           nameLower.contains("level") ||
                           nameLower.contains("state") ||
                           nameLower.contains("select") ||
                           nameLower.contains("switch") ||
                           nameLower.contains("position") && !nameLower.contains("pedal");

    if (isDiscreteLike && hasStateKeyword) {
        meta.type = SignalType::State;
        meta.recommendedPlot = PlotRecommendation::DigitalState;
        return meta;
    }

    meta.type = SignalType::Analog;
    meta.recommendedPlot = PlotRecommendation::TimeBased;

    return meta;
}

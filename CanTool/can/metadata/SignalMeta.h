#pragma once

#include <QString>
#include <QStringList>
#include <QVariant>


enum class SignalType
{
    Unknown = 0,
    Analog,
    Digital,
    State,
    XY
};


enum class PlotRecommendation
{
    None,
    TimeBased,
    DigitalState,
    XYCompatible
};


struct SignalMeta
{
    QString signalId;
    SignalType type = SignalType::Unknown;

    QString unit;
    QString displayName;

    // Jus use this property when type == State
    QStringList stateLabels;    // index 0 → "P", index 1 → "R", ... (from DBC ValTable)

    PlotRecommendation recommendedPlot = PlotRecommendation::None;

    bool defaultVisible = true;
    double defaultMin = 0.0;
    double defaultMax = 0.0;
    bool isMultiplexed = false;


    SignalMeta() = default;


    SignalMeta(const QString& id, SignalType t, const QString& u = {}, const QString& name = {})
        : signalId(id), type(t), unit(u), displayName(name)
    {
        if (type == SignalType::Analog) {
            recommendedPlot = PlotRecommendation::TimeBased;
            defaultVisible = true;
        }
        else if (type == SignalType::Digital || type == SignalType::State) {
            recommendedPlot = PlotRecommendation::DigitalState;
            defaultVisible = true;
        }
    }
};

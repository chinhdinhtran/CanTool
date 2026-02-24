#ifndef PLOTCONTROLLER_H
#define PLOTCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QString>

#include "can/runtime/SignalStore.h"
#include "can/metadata/SignalMeta.h"
#include <QColor>

class SignalPlotWidget;
class QWidget;

/**
 * @brief PlotController
 * Chooses plot type based on SignalMeta and manages plot widgets
 */
class PlotController : public QObject
{
    Q_OBJECT
public:
    explicit PlotController(SignalStore* store,
                            QObject* parent = nullptr);

    // Add a signal to UI (auto choose plot)
    SignalPlotWidget* addSignal(const QString& signalId,
                                const SignalMeta& meta,
                                QWidget* parentWidget);
    SignalPlotWidget* findPlot(const QString& signalId) const;
    QList<SignalPlotWidget*> plots() const;
    QColor colorForSignal(const QString& id);
    void clear();

private:
    SignalStore* m_store;

    // One plot per signal (simple & clear for now)
    QHash<QString, SignalPlotWidget*> m_signalPlots;
    QList<QColor> m_colorPalette;
    int m_nextColorIndex = 0;
    QHash<QString, QColor> m_signalColors;

    QColor nextColor();
};

#endif // PLOTCONTROLLER_H

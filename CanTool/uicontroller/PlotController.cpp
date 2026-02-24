#include "PlotController.h"

#include "ui/plot/SignalPlotWidget.h"
#include "ui/plot/TimeSignalPlot.h"
#include "ui/plot/DigitalSignalPlot.h"
#include "ui/plot/XYPlotWidget.h"

#include <QDebug>

PlotController::PlotController(SignalStore* store,
                               QObject* parent)
    : QObject(parent),
    m_store(store)
{
    m_colorPalette = {
        QColor(0xff, 0x52, 0x52), // red
        QColor(0x21, 0x96, 0xf3), // blue
        QColor(0xff, 0xeb, 0x3b), // yellow
        QColor(0x4c, 0xaf, 0x50), // green
        QColor(0xe9, 0x1e, 0x63), // pink
        QColor(0xff, 0x98, 0x00), // orange
        QColor(0x00, 0xbc, 0xd4), // cyan
        QColor(0x9c, 0x27, 0xb0)  // purple
    };
}

QColor PlotController::nextColor()
{
    QColor c = m_colorPalette[m_nextColorIndex % m_colorPalette.size()];
    ++m_nextColorIndex;
    return c;
}

SignalPlotWidget* PlotController::addSignal(const QString& signalId,
                          const SignalMeta& meta,
                          QWidget* parentWidget)
{
    // Already exists
    if (m_signalPlots.contains(signalId))
        return m_signalPlots.value(signalId);

    SignalPlotWidget* plot = nullptr;

    switch (meta.type)
    {
    case SignalType::Analog:
        plot = new TimeSignalPlot(m_store, parentWidget);
        break;

    case SignalType::Digital:
    case SignalType::State:
        plot = new DigitalSignalPlot(m_store, parentWidget);
        break;

    case SignalType::XY:
        plot = new XYPlotWidget(m_store, parentWidget);
        break;

    default:
        qWarning() << "Unknown SignalType for" << signalId;
        return nullptr;
    }

    QColor color = colorForSignal(signalId);
    plot->setProperty("plotColor", color);

    plot->addSignal(signalId);
    plot->setWindowTitle(meta.displayName);

    m_signalPlots.insert(signalId, plot);
    return plot;
}

SignalPlotWidget *PlotController::findPlot(const QString &signalId) const
{
    return m_signalPlots.value(signalId, nullptr);
}

QList<SignalPlotWidget *> PlotController::plots() const
{
    return m_signalPlots.values();
}

QColor PlotController::colorForSignal(const QString& id)
{
    if (!m_signalColors.contains(id))
        m_signalColors[id] = nextColor();
    return m_signalColors[id];
}

void PlotController::clear()
{
    qDeleteAll(m_signalPlots);
    m_signalPlots.clear();
    m_signalColors.clear();
    m_nextColorIndex = 0;
}

#include "TimeSignalPlot.h"
#include "can/runtime/SignalStore.h"

#include <QVBoxLayout>

TimeSignalPlot::TimeSignalPlot(SignalStore* store, QWidget* parent)
    : SignalPlotWidget(parent),
    m_store(store)
{
    m_plot = new QCustomPlot(this);

    // turn on interaction
    m_plot->setInteractions(
        QCP::iRangeZoom |
        QCP::iRangeDrag
        );

    // Zoom & drag follow X (time)
    m_plot->axisRect()->setRangeZoom(Qt::Horizontal);
    m_plot->axisRect()->setRangeDrag(Qt::Horizontal);
    m_plot->axisRect()->setRangeZoomFactor(0.85);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_plot);

    m_plot->legend->setVisible(true);   // set tag singal -- so heavy
    m_plot->setNotAntialiasedElements(QCP::aeAll);
    m_plot->setPlottingHint(QCP::phFastPolylines);

    m_plot->xAxis->setLabel("Time (s)");
    m_plot->yAxis->setLabel("Value");
    m_plot->installEventFilter(this);

    connect(m_plot, &QCustomPlot::mousePress,
            this, [this]() {
                m_autoFollowX = false;
            });

    connect(m_plot, &QCustomPlot::mouseDoubleClick,
            this, [this]() {
                emit requestPlay();
                m_autoFollowX = true;
                m_plot->rescaleAxes();
                m_plot->replot();
            });
}

bool TimeSignalPlot::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_plot && event->type() == QEvent::Wheel)
    {
        auto* wheel = static_cast<QWheelEvent*>(event);

        if (wheel->modifiers() & Qt::ControlModifier)
            return false; // QCustomPlot zoom

        // forward for scroll area
        auto* scrollArea = parentWidget()->parentWidget();
        if (scrollArea)
            QCoreApplication::sendEvent(scrollArea, event);

        return true;
    }
    return false;
}

void TimeSignalPlot::addSignal(const QString& signalId)
{
    if (m_graphs.contains(signalId))
        return;

    QCPGraph* graph = m_plot->addGraph();
    graph->setName(signalId);

    QColor color = property("plotColor").value<QColor>();
    if (color.isValid())
    {
        QPen pen(color);
        pen.setWidth(1);
        graph->setPen(pen);
    }

    m_graphs.insert(signalId, graph);
}

void TimeSignalPlot::refresh()
{
    for (auto it = m_graphs.begin(); it != m_graphs.end(); ++it)
    {
        const auto* buffer = m_store->buffer(it.key());
        const auto& data = buffer->data();

        QVector<double> x;
        QVector<double> y;
        x.reserve(data.size());
        y.reserve(data.size());

        for (const auto& s : data)
        {
            x.push_back(s.timestamp);
            y.push_back(s.value);
        }

        it.value()->setData(x, y);
    }

    if (m_autoFollowX)
        m_plot->rescaleAxes();   // just auto when no zoom

    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void TimeSignalPlot::setAutoFollowX(bool enable)
{
    m_autoFollowX = enable;

    if (enable)
    {
        m_plot->rescaleAxes();
        m_plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

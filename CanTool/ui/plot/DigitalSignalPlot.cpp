#include "DigitalSignalPlot.h"
#include "can/runtime/SignalStore.h"

#include <QVBoxLayout>

DigitalSignalPlot::DigitalSignalPlot(SignalStore* store, QWidget* parent)
    : SignalPlotWidget(parent),
    m_store(store)
{
    m_plot = new QCustomPlot(this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_plot);

    m_plot->legend->setVisible(true);
    m_plot->yAxis->setRange(-0.5, 1.5);
    m_plot->yAxis->setLabel("State");
    m_plot->xAxis->setLabel("Time (s)");
}

void DigitalSignalPlot::addSignal(const QString& signalId)
{
    if (m_graphs.contains(signalId))
        return;

    QCPGraph* graph = m_plot->addGraph();
    graph->setName(signalId);
    graph->setLineStyle(QCPGraph::lsStepLeft);
    m_graphs.insert(signalId, graph);
}

void DigitalSignalPlot::refresh()
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
            y.push_back(s.value > 0.5 ? 1.0 : 0.0);
        }

        it.value()->setData(x, y);
    }

    m_plot->rescaleAxes();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

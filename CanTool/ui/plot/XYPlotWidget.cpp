#include "XYPlotWidget.h"
#include "can/runtime/SignalStore.h"

#include <QVBoxLayout>

XYPlotWidget::XYPlotWidget(SignalStore* store, QWidget* parent)
    : SignalPlotWidget(parent),
    m_store(store)
{
    m_plot = new QCustomPlot(this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_plot);

    m_plot->xAxis->setLabel("X");
    m_plot->yAxis->setLabel("Y");
}

void XYPlotWidget::addSignal(const QString& signalId)
{
    if (m_xSignal.isEmpty())
    {
        m_xSignal = signalId;
        return;
    }

    if (m_ySignal.isEmpty())
    {
        m_ySignal = signalId;
        m_graph = m_plot->addGraph();
        m_graph->setLineStyle(QCPGraph::lsNone);
        m_graph->setScatterStyle(QCPScatterStyle::ssDisc);
    }
}

void XYPlotWidget::refresh()
{
    if (!m_graph || m_xSignal.isEmpty() || m_ySignal.isEmpty())
        return;

    const auto* xBuf = m_store->buffer(m_xSignal);
    const auto* yBuf = m_store->buffer(m_ySignal);

    const auto& xData = xBuf->data();
    const auto& yData = yBuf->data();

    int n = std::min(xData.size(), yData.size());

    QVector<double> x;
    QVector<double> y;
    x.reserve(n);
    y.reserve(n);

    for (int i = 0; i < n; ++i)
    {
        x.push_back(xData[i].value);
        y.push_back(yData[i].value);
    }

    // m_graph->setData(x, y); //perf
    m_graph->addData(x, y);
    m_plot->rescaleAxes();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

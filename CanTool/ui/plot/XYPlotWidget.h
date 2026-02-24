#pragma once

#include "SignalPlotWidget.h"
#include <qcustomplot.h>

class SignalStore;

class XYPlotWidget : public SignalPlotWidget
{
    Q_OBJECT
public:
    explicit XYPlotWidget(SignalStore* store,
                          QWidget* parent = nullptr);

    void addSignal(const QString& signalId) override;
    void refresh() override;

private:
    SignalStore* m_store;
    QCustomPlot* m_plot;

    QString m_xSignal;
    QString m_ySignal;
    QCPGraph* m_graph = nullptr;
};

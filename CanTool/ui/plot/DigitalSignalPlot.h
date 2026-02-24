#pragma once

#include "SignalPlotWidget.h"
#include <QHash>
#include <qcustomplot.h>

class SignalStore;

class DigitalSignalPlot : public SignalPlotWidget
{
    Q_OBJECT
public:
    explicit DigitalSignalPlot(SignalStore* store,
                               QWidget* parent = nullptr);

    void addSignal(const QString& signalId) override;
    void refresh() override;

private:
    SignalStore* m_store;
    QCustomPlot* m_plot;
    QHash<QString, QCPGraph*> m_graphs;
};

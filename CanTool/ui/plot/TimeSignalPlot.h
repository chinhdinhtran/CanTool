#pragma once

#include "SignalPlotWidget.h"
#include <QHash>
#include <qcustomplot.h>

class SignalStore;

class TimeSignalPlot : public SignalPlotWidget
{
    Q_OBJECT
public:
    explicit TimeSignalPlot(SignalStore* store,
                            QWidget* parent = nullptr);

    void addSignal(const QString& signalId) override;
    void refresh() override;
    void setAutoFollowX(bool enable) override;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    bool m_autoFollowX = true;

private:
    SignalStore* m_store;
    QCustomPlot* m_plot;
    QHash<QString, QCPGraph*> m_graphs;
};

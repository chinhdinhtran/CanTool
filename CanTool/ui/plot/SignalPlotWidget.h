#pragma once

#include <QWidget>
#include <QString>

/**
 * @brief SignalPlotWidget
 * Abstract base class for all signal plot widgets
 */
class SignalPlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalPlotWidget(QWidget* parent = nullptr)
        : QWidget(parent) {}

    virtual ~SignalPlotWidget() = default;

    virtual void addSignal(const QString& signalId) = 0;

    virtual void refresh() = 0;

    virtual void setAutoFollowX(bool enable) { Q_UNUSED(enable); }

signals:
    void requestPlay();

};

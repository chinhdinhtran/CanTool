#ifndef GRAPHICWIDGET_H
#define GRAPHICWIDGET_H

#include <QMainWindow>
#include <QTimer>

#include "can/runtime/SignalStore.h"
#include "uicontroller/PlotController.h"
#include "ui/plot/TimeSignalPlot.h"
#include "ui/plot/SignalPlotWidget.h"
#include "ui/plot/XYPlotWidget.h"
#include "can/model/CanSignalSearch.h"
#include "can/filter/CanSignalFilterModel.h"
#include <QElapsedTimer>
#include <QSet>
#include <QTimer>

namespace Ui {
class GraphicWidget;
}

class GraphicWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphicWidget(const CanInformation* canInfo, SignalStore* store, QWidget *parent = nullptr);
    ~GraphicWidget();
    void showPanel();
    void hidePanel();

signals:
    void homeClicked();

public slots:
    void reloadSignalModel();
    void clearSignalModel();

    void on_homeButtonClicked();
    void onPauseClicked();
    void onPlayClicked();
    void onSignalDoubleClicked(const QModelIndex& index);
    void onSelectedTableItemChanged(QTableWidgetItem* item);

private:
    void setupToolBar();
    void setupControllers();
    void setupSignalDock();
    void setupPlotDock();
    void clearAllPlots();

    SignalMeta dummyMetaForSignal(const QString& signalId);
    void startDummySignal(QString name="");
    void dummySignals(int num);
    void addSignalToView(const QString& signalId);
    QWidget* createStateWidget(const QColor& color, bool checked, const QString& signalId);

    Ui::GraphicWidget *ui;
    const CanInformation* m_canInfo = nullptr;
    SignalStore* m_signalStore = nullptr;
    PlotController* m_plotController = nullptr;

    QDockWidget *m_signalDock = nullptr;
    QTreeView *m_signalTree = nullptr;
    QLineEdit *searchEdit = nullptr;
    CanSignalSearch* m_signalModel = nullptr;
    QTableWidget* m_selectedTable = nullptr;
    QHash<QString, int> m_signalRow;
    QDockWidget* m_plotDock = nullptr;
    QWidget*     m_plotContainer = nullptr;
    QVBoxLayout* m_plotLayout = nullptr;
    QTimer* m_refreshTimer = nullptr;

    QHash<QString, QTimer*> m_dummyTimers;
    QElapsedTimer m_elapsed;

    bool m_isPaused = false;
};

#endif // GRAPHICWIDGET_H

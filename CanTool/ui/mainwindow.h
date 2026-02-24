#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "ui/graphicswindow/graphicwidget.h"
#include "ui/tracewindow/tracewidget.h"

#include "CanController.h"
#include "can/runtime/SignalStore.h"




QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpenDbc_clicked();
    void on_testDbcLog_clicked();
    void on_btnClearDbc_clicked();

    void showMainPage();
    void on_disp_graphic_clicked();
    void on_showTraceButton_clicked();

signals:
    void dbcLoaded();
    void clearDbcSearchModel();
private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    QWidget *mainPage;
    GraphicWidget *graphicPage;
    TraceWidget *tracePage;




    CanController* m_canController = nullptr;
    SignalStore* m_signalStore = nullptr;

};
#endif // MAINWINDOW_H

#ifndef TRACEWIDGET_H
#define TRACEWIDGET_H

#include <QWidget>

namespace Ui {
class TraceWidget;
}

class TraceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TraceWidget(QWidget *parent = nullptr);
    ~TraceWidget();

signals:
    void homeClicked();
private slots:
    void on_homeButton_clicked();

private:
    Ui::TraceWidget *ui;
};

#endif // TRACEWIDGET_H

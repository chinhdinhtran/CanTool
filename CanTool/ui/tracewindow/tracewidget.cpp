#include "tracewidget.h"
#include "ui/tracewindow/ui_tracewidget.h"

TraceWidget::TraceWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TraceWidget)
{
    ui->setupUi(this);
}

TraceWidget::~TraceWidget()
{
    delete ui;
}

void TraceWidget::on_homeButton_clicked()
{
    emit homeClicked();
}


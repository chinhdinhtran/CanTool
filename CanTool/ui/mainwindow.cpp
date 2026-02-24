#include "mainwindow.h"
#include "ui/ui_mainwindow.h"
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_signalStore = new SignalStore(this);
    m_canController = new CanController(m_signalStore, this);

    QWidget *oldCentral = takeCentralWidget();
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    mainPage = oldCentral;
    stackedWidget->addWidget(mainPage);

    // Page graphic
    graphicPage = new GraphicWidget(
        &m_canController->model(),
        m_signalStore,
        this
        );
    stackedWidget->addWidget(graphicPage);
    // Page trace
    tracePage = new TraceWidget(this);
    stackedWidget->addWidget(tracePage);


    stackedWidget->setCurrentWidget(mainPage);

    connect(graphicPage, &GraphicWidget::homeClicked,
            this, &MainWindow::showMainPage);

    connect(tracePage, &TraceWidget::homeClicked,
            this, &MainWindow::showMainPage);

    connect(this, &MainWindow::dbcLoaded,
            graphicPage, &GraphicWidget::reloadSignalModel);
    connect(this, &MainWindow::clearDbcSearchModel,
            graphicPage, &GraphicWidget::clearSignalModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOpenDbc_clicked()
{
    qDebug() << Q_FUNC_INFO;
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Open DBC Files"),
        QString(),
        tr("DBC Files (*.dbc)")
        );

    if (filePaths.isEmpty())
        return;

    for (const QString& path : filePaths)
    {
        if (!m_canController->loadDbc(path))
        {
            ui->editDbcPath->setText("DBC loads fail");
            QMessageBox::critical(
                this,
                tr("DBC Error"),
                tr("Failed to parse DBC file:\n%1")
                    .arg(QFileInfo(path).fileName())
                );
            return;
        }
    }
    emit dbcLoaded();
    ui->editDbcPath->setText("DBC loads successfuly.");

    m_canController->openCan();
    m_canController->start();
}

void MainWindow::on_testDbcLog_clicked()
{
    qDebug() << Q_FUNC_INFO;

    const CanInformation& info = m_canController->model();
    const auto& messages = info.messages();

    int totalSignalCount = 0;

    qDebug() << "===== DBC MODEL DUMP =====";

    for (auto it = messages.constBegin(); it != messages.constEnd(); ++it)
    {
        const CanMessageInfo& msg = it.value();

        qDebug().noquote()
            << QString("MSG  ID=0x%1  Name=%2  DLC=%3")
                   .arg(msg.id, 0, 16)
                   .arg(msg.name)
                   .arg(msg.dlc);

        totalSignalCount += msg.signalList.size();

        for (const auto& sig : msg.signalList)
        {
            qDebug().noquote()
            << QString("   SIG  %1  bit=%2|%3  %4  scale=%5 offset=%6  [%7..%8] %9")
                    .arg(sig.name.leftJustified(20))   // %1
                    .arg(sig.startBit)                 // %2
                    .arg(sig.bitLength)                // %3
                    .arg(sig.isSigned ? "signed" : "unsigned") // %4
                    .arg(sig.scale)                    // %5
                    .arg(sig.offset)                   // %6
                    .arg(sig.minValue)                 // %7
                    .arg(sig.maxValue)                 // %8
                    .arg(sig.unit);                    // %9
        }
    }

    qDebug() << "Message count:" << messages.size() << "\nSignal count :" << totalSignalCount;
    qDebug() << "===== END DBC MODEL DUMP =====";
}




void MainWindow::on_btnClearDbc_clicked()
{
    qDebug() << Q_FUNC_INFO;
    m_canController->clearModel();
    emit clearDbcSearchModel();
    ui->editDbcPath->setText("DBC was clear!");
}

void MainWindow::on_disp_graphic_clicked()
{
    graphicPage->showPanel();
    stackedWidget->setCurrentWidget(graphicPage);
}

void MainWindow::showMainPage()
{
    stackedWidget->setCurrentWidget(mainPage);
}

void MainWindow::on_showTraceButton_clicked()
{
    stackedWidget->setCurrentWidget(tracePage);
}


#include "graphicwidget.h"
#include "ui/graphicswindow/ui_graphicwidget.h"

#include <cmath>

GraphicWidget::GraphicWidget(const CanInformation* canInfo, SignalStore* store, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphicWidget)
    , m_signalStore(store)
    , m_canInfo(canInfo)
{
    ui->setupUi(this);

    setupToolBar();
    setupControllers();
    setupSignalDock();
    setupPlotDock();
}

GraphicWidget::~GraphicWidget()
{
    delete ui;
}

void GraphicWidget::showPanel()
{
    qDebug() << Q_FUNC_INFO;
    if (m_signalDock)
        m_signalDock->show();
}

void GraphicWidget::hidePanel()
{
    qDebug() << Q_FUNC_INFO;
    if (searchEdit)
        searchEdit->clear();

    if (m_signalTree)
        m_signalTree->expandAll();

    if (m_signalDock)
        m_signalDock->hide();
}

void GraphicWidget::on_homeButtonClicked()
{
    qDebug() << Q_FUNC_INFO;
    hidePanel();
    emit homeClicked();
}

void GraphicWidget::onPauseClicked()
{
    qDebug() << Q_FUNC_INFO;
    m_isPaused = true;
}

void GraphicWidget::onPlayClicked()
{
    qDebug() << Q_FUNC_INFO;
    m_isPaused = false;
    for (auto* plot : m_plotController->plots())
        plot->setAutoFollowX(true);
}

void GraphicWidget::reloadSignalModel()
{
    if (m_signalModel){
        qDebug() << Q_FUNC_INFO;
        m_signalModel->reload();
    }
}

void GraphicWidget::clearSignalModel()
{
    qDebug() << Q_FUNC_INFO;

    // ===== 1. Clear search model =====
    if (m_signalModel)
        m_signalModel->clear();

    // ===== 2. Clear selected table =====
    if (m_selectedTable)
    {
        m_selectedTable->blockSignals(true);
        m_selectedTable->setRowCount(0);
        m_selectedTable->blockSignals(false);
    }

    m_signalRow.clear();

    // ===== 3. Clear plots =====
    clearAllPlots();

    // ===== 4. Clear runtime data =====
    if (m_signalStore)
        m_signalStore->clear();
}

void GraphicWidget::setupControllers()
{
    m_plotController = new PlotController(m_signalStore, this);
}

void GraphicWidget::setupSignalDock()
{
    m_signalDock = new QDockWidget("Signals", this);
    m_signalDock->setFeatures(
        QDockWidget::DockWidgetMovable |
        // QDockWidget::DockWidgetClosable |
        QDockWidget::DockWidgetFloatable
        );

    auto* container = new QWidget(m_signalDock);

    auto* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // ===== SEARCH =====
    searchEdit = new QLineEdit(container);
    searchEdit->setPlaceholderText("Search CAN Signal...");
    searchEdit->setClearButtonEnabled(true);

    mainLayout->addWidget(searchEdit);

    // ===== SPLITTER =====
    auto* splitter = new QSplitter(Qt::Vertical, container);

    // ---- Signal tree (2/3) ----
    m_signalTree = new QTreeView(splitter);

    // ---- Selected table (1/3) ----
    m_selectedTable = new QTableWidget(splitter);
    m_selectedTable->setColumnCount(2);
    m_selectedTable->setHorizontalHeaderLabels({ "State", "Signal" });
    auto* header = m_selectedTable->horizontalHeader();
    header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_selectedTable->horizontalHeader()->setStretchLastSection(true);
    m_selectedTable->verticalHeader()->setVisible(false);
    m_selectedTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_selectedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    splitter->addWidget(m_signalTree);
    splitter->addWidget(m_selectedTable);

    // 2/3 – 1/3
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter, 1);

    // ===== MODEL =====
    m_signalModel = new CanSignalSearch(m_canInfo, this);

    auto* filterModel = new CanSignalFilterModel(this);
    filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterModel->setSourceModel(m_signalModel);

    m_signalTree->setModel(filterModel);
    m_signalTree->setHeaderHidden(true);
    m_signalTree->setStyleSheet(R"(
        QTreeView::item:hover {
            background-color: #2a82da;
            color: white;
        }
        )");

    connect(searchEdit, &QLineEdit::textChanged,
            filterModel, [=](const QString &text) {
                filterModel->setFilterRegularExpression(
                    QRegularExpression(text, QRegularExpression::CaseInsensitiveOption)
                    );
            });

    connect(m_signalTree, &QTreeView::doubleClicked,
            this, &GraphicWidget::onSignalDoubleClicked);

    connect(m_selectedTable, &QTableWidget::itemChanged,
            this, &GraphicWidget::onSelectedTableItemChanged);

    // ===== SHORTCUT =====
    auto* shortcut = new QShortcut(QKeySequence::Find, this);
    connect(shortcut, &QShortcut::activated,
            searchEdit, QOverload<>::of(&QLineEdit::setFocus));

    m_signalDock->setWidget(container);
    addDockWidget(Qt::LeftDockWidgetArea, m_signalDock);
}

void GraphicWidget::setupToolBar()
{
    QToolBar* toolbar = addToolBar("Control");

    QAction* homeAction = toolbar->addAction("🏠 Home");
    connect(homeAction, &QAction::triggered,
            this, &GraphicWidget::on_homeButtonClicked);

    QAction* playAction = toolbar->addAction("▶ Play");
    QAction* pauseAction = toolbar->addAction("⏸ Pause");

    toolbar->addSeparator();

    connect(playAction, &QAction::triggered,
            this, &GraphicWidget::onPlayClicked);

    connect(pauseAction, &QAction::triggered,
            this, &GraphicWidget::onPauseClicked);
}

void GraphicWidget::addSignalToView(const QString& signalId)
{
    if (m_signalRow.contains(signalId))
        return;

    m_selectedTable->blockSignals(true);

    int row = m_selectedTable->rowCount();
    m_selectedTable->insertRow(row);

    auto* checkItem = new QTableWidgetItem();
    checkItem->setCheckState(Qt::Checked);
    checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    auto* nameItem = new QTableWidgetItem(signalId);

    m_selectedTable->setItem(row, 0, checkItem);
    m_selectedTable->setItem(row, 1, nameItem);

    m_signalRow.insert(signalId, row);

    m_selectedTable->blockSignals(false);

    // Dummy data
    startDummySignal(signalId);

    // Plot
    SignalMeta meta = dummyMetaForSignal(signalId);
    auto* plot = m_plotController->addSignal(signalId, meta, m_plotContainer);
    plot->setFixedHeight(200);

    m_plotLayout->insertWidget(m_plotLayout->count() - 1, plot);
    plot->show();
}

void GraphicWidget::dummySignals(int num)
{
    if (!m_signalModel)
        return;

    const int totalSignals = m_signalModel->rowCount();
    const int maxCount =
        (num <= 0 || num > totalSignals) ? totalSignals : num;

    m_selectedTable->blockSignals(true);
    m_selectedTable->setRowCount(0);
    m_signalRow.clear();
    m_selectedTable->blockSignals(false);

    for (int row = 0; row < maxCount; ++row)
    {
        QModelIndex idx = m_signalModel->index(row, 0);
        if (!idx.isValid())
            continue;

        QString signalId =
            m_signalModel->data(idx, Qt::DisplayRole).toString();

        if (signalId.isEmpty())
            continue;

        addSignalToView(signalId);
    }
}
void GraphicWidget::onSignalDoubleClicked(const QModelIndex& index)
{
    // test 500 signals
    // dummySignals(500);

    if (!index.isValid())
        return;

    auto* proxy =
        qobject_cast<QSortFilterProxyModel*>(m_signalTree->model());
    if (!proxy)
        return;

    QModelIndex srcIndex = proxy->mapToSource(index);
    if (!srcIndex.isValid())
        return;

    QString signalId =
        m_signalModel->data(srcIndex, Qt::DisplayRole).toString();
    if (signalId.isEmpty())
        return;

    if (m_signalRow.contains(signalId))
        return;

    m_selectedTable->blockSignals(true);

    int row = m_selectedTable->rowCount();
    m_selectedTable->insertRow(row);

    auto* checkItem = new QTableWidgetItem();
    checkItem->setCheckState(Qt::Checked);
    checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    auto* nameItem = new QTableWidgetItem(signalId);

    m_selectedTable->setItem(row, 0, checkItem);
    m_selectedTable->setItem(row, 1, nameItem);

    m_signalRow.insert(signalId, row);

    m_selectedTable->blockSignals(false);

    // ===== DUMMY DATA =====
    // startDummySignal(signalId);

    // ===== PLOT =====
    SignalMeta meta = dummyMetaForSignal(signalId);

    SignalPlotWidget* plot = m_plotController->addSignal(signalId, meta, m_plotContainer);
    plot->setFixedHeight(200);
    m_plotLayout->insertWidget(m_plotLayout->count() - 1, plot);

    connect(plot, &SignalPlotWidget::requestPlay,
            this, &GraphicWidget::onPlayClicked);

    plot->setVisible(false);    //performance better than plot->show()
    QTimer::singleShot(0, this, [plot]() {
        plot->setVisible(true);
    });
}

void GraphicWidget::onSelectedTableItemChanged(QTableWidgetItem* item)
{
    if (!item || item->column() != 0)
        return;

    QString signalId =
        m_selectedTable->item(item->row(), 1)->text();

    auto* plot = m_plotController->findPlot(signalId);
    if (!plot)
        return;

    if (item->checkState() == Qt::Checked)
        plot->show();
    else
        plot->hide();
}

SignalMeta GraphicWidget::dummyMetaForSignal(const QString& signalId)
{
    SignalMeta meta;
    meta.displayName = signalId;
    meta.type = SignalType::Analog;
    return meta;
}


void GraphicWidget::setupPlotDock()
{
    m_elapsed.start();
    m_plotDock = new QDockWidget("Graphics", this);
    m_plotDock->setFeatures(
        QDockWidget::DockWidgetMovable |
        // QDockWidget::DockWidgetClosable |
        QDockWidget::DockWidgetFloatable
        );

    // ===== Scroll Area =====
    auto* scrollArea = new QScrollArea(m_plotDock);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // ===== Container =====
    m_plotContainer = new QWidget(scrollArea);
    m_plotLayout = new QVBoxLayout(m_plotContainer);
    m_plotLayout->setContentsMargins(4, 4, 4, 4);
    m_plotLayout->setSpacing(6);
    m_plotLayout->addStretch();

    scrollArea->setWidget(m_plotContainer);
    m_plotDock->setWidget(scrollArea);

    m_plotDock->setStyleSheet(R"(
    QWidget {
        border: 1px solid #333;
        border-radius: 4px;
    }
)");

    addDockWidget(Qt::RightDockWidgetArea, m_plotDock);

    // ===== Refresh timer =====
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, [this]() {
        if (m_isPaused)
            return;
        for (auto* plot : m_plotController->plots())
            if (plot->isVisible())
                plot->refresh();
    });
    m_refreshTimer->start(50);
}

void GraphicWidget::clearAllPlots()
{
    if (!m_plotLayout)
        return;

    while (m_plotLayout->count() > 1)
    {
        QLayoutItem* item = m_plotLayout->takeAt(0);
        if (!item)
            continue;

        QWidget* w = item->widget();
        if (w)
            w->deleteLater();

        delete item;
    }
    if (m_plotController)
        m_plotController->clear();
}

void GraphicWidget::startDummySignal(QString name)
{
    if (m_isPaused)
        return;



    // DUMMY ANALOG
    // if (m_dummyTimers.contains(name))
    //     return;

    // auto* timer = new QTimer(this);
    // m_dummyTimers.insert(name, timer);

    // connect(timer, &QTimer::timeout, this, [this, name]() {
    //     // if (m_isPaused) //still receive data on pause time
    //     //     return;
    //     double time = m_elapsed.elapsed() / 1000.0;
    //     double value = 800.0 + 400.0 * std::sin(time);
    //     m_signalStore->appendSample(name, time, value);
    // });

    // timer->start(50);



    // // DUMMY ANALOG
    // if (m_dummyTimers.contains(name))
    //     return;

    // auto* timer = new QTimer(this);
    // m_dummyTimers.insert(name, timer);
    // double value = QRandomGenerator::global()->bounded(1000.0);
    // connect(timer, &QTimer::timeout, this, [this, name]() {

    //     double time = m_elapsed.elapsed() / 1000.0;
    //     double value = QRandomGenerator::global()->bounded(1000.0);

    //     m_signalStore->appendSample(name, time, value);
    // });
    // timer->start(50);







    // // DUMMY ANALOG
    if (m_dummyTimers.contains(name))
        return;

    auto* timer = new QTimer(this);
    m_dummyTimers.insert(name, timer);
    static QHash<QString, double> value;
    static QHash<QString, double> nextChangeTime;

    if (!value.contains(name))
        value[name] = QRandomGenerator::global()->bounded(1000.0);

    if (!nextChangeTime.contains(name))
        nextChangeTime[name] = 0.0;
    connect(timer, &QTimer::timeout, this, [this, name]() {

        double time = m_elapsed.elapsed() / 1000.0;

        if (time >= nextChangeTime[name])
        {
            value[name] = QRandomGenerator::global()->bounded(1000.0);

            double hold =
                QRandomGenerator::global()->bounded(200, 1000) / 1000.0;

            nextChangeTime[name] = time + hold;
        }

        m_signalStore->appendSample(name, time, value[name]);
    });
    timer->start(50);







    // // DUMMY DIGITAL
    // if (m_dummyTimers.contains(name))
    //     return;

    // auto* timer = new QTimer(this);
    // m_dummyTimers.insert(name, timer);

    // static QHash<QString, double> state;
    // static QHash<QString, double> nextToggleTime;

    // // init
    // if (!state.contains(name))
    //     state[name] = (QRandomGenerator::global()->bounded(2) == 0) ? 0.5 : 1.0;

    // if (!nextToggleTime.contains(name))
    //     nextToggleTime[name] = 0.0;

    // connect(timer, &QTimer::timeout, this, [this, name]() {

    //     double time = m_elapsed.elapsed() / 1000.0;

    //     if (time >= nextToggleTime[name])
    //     {
    //         state[name] = (state[name] == 1.0) ? 0.5 : 1.0;
    //         double hold =
    //             QRandomGenerator::global()->bounded(300, 3000) / 1000.0;

    //         nextToggleTime[name] = time + hold;
    //     }

    //     m_signalStore->appendSample(name, time, state[name]);
    // });

    // timer->start(50);
}



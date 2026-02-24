#include "CanController.h"
#include "can/dbc/DbcLibWrapper.h"
#include "../utils/TimeUtils.h"
#include <QTimer>
#include <cmath>
#include "CanController.h"

#include <QDebug>

#include "can/driver/VectorCanDriver.h"
#include "can/receive/CanReceiver.h"
// #include "can/log/CanLogWriter.h"
#include "can/runtime/SignalStore.h"

CanController::CanController(SignalStore* store, QObject* parent)
    : QObject(parent),
    m_store(store)
{

}

CanController::~CanController()
{
    closeCan();
}

// ======================
// CAN DEVICE
// ======================
bool CanController::openCan()
{
    if (m_canOpened)
        return true;

    m_driver = new VectorCanDriver();
    if (!m_driver->open())
    {
        qWarning() << "Failed to open Vector CAN";
        delete m_driver;
        m_driver = nullptr;
        return false;
    }

    m_rx = new CanReceiver(m_driver, &m_dbcManager, this);
    connect(m_rx, &CanReceiver::decodedSignal,
            m_store, &SignalStore::append,
            Qt::QueuedConnection);


    m_logger = new CanLogWriter(this);
    connect(m_rx, &CanReceiver::rawFrameReceived,
            m_logger, &CanLogWriter::onFrame,
            Qt::QueuedConnection);


    m_canOpened = true;
    qDebug() << "CAN thread opened";
    return true;
}

void CanController::closeCan()
{
    if (!m_canOpened)
        return;

    if (m_rx && m_rx->isRunning()) {
        m_rx->requestInterruption();
        m_rx->wait();
        delete m_rx;
        m_rx = nullptr;
    }

    if (m_logger) {
        m_logger->stopLogging();
        delete m_logger;
        m_logger = nullptr;
    }

    if (m_driver) {
        m_driver->close();
        delete m_driver;
        m_driver = nullptr;
    }

    m_canOpened = false;
    qDebug() << "CAN closed";
}

// ======================
// RX CONTROL
// ======================
void CanController::start()
{
    if (!m_canOpened)
    {
        qWarning() << "CAN not opened";
        return;
    }

    if (!m_rx)
    {
        qWarning() << "RX thread not initialized";
        return;
    }

    if (!m_rx->isRunning())
    {
        m_rx->start();
        qDebug() << "CAN RX started";
    }

    if (!m_logger->isRunning())
    {
        m_logger->startLogging();
    }
}

bool CanController::isRunning() const
{
    return m_rx && m_rx->isRunning();
}

bool CanController::loadDbc(const QString& path)
{
    if (!m_dbcManager.loadFile(path))
        return false;

    qDebug() << "DBC loaded:" << path;
    return true;
}

const CanInformation& CanController::model() const
{
    return m_dbcManager.canInformation();
}

void CanController::clearModel()
{
    qDebug() << Q_FUNC_INFO;
    m_dbcManager.clearModel();
}

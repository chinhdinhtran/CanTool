#include "CanLogWriter.h"
#include <QDebug>

CanLogWriter::CanLogWriter(QObject* parent)
    : QThread(parent)
{
}

CanLogWriter::~CanLogWriter()
{
    stopLogging();
}

void CanLogWriter::startLogging()
{
    qDebug() << Q_FUNC_INFO;
    if (m_running)
        return;

    m_running = true;
    m_fileMgr.open();
    start();
}

void CanLogWriter::stopLogging()
{
    {
        QMutexLocker locker(&m_mutex);
        m_running = false;
        m_cond.wakeAll();
    }
    wait();
    m_fileMgr.close();
}

void CanLogWriter::onFrame(const CanRawFrame& frame)
{
    QMutexLocker locker(&m_mutex);
    if (!m_running)
        return;

    m_queue.enqueue(frame);
    m_cond.wakeOne();
}

void CanLogWriter::run()
{
    while (true) {
        CanRawFrame frame;

        {
            QMutexLocker locker(&m_mutex);
            if (!m_running && m_queue.isEmpty())
                break;

            if (m_queue.isEmpty()) {
                m_cond.wait(&m_mutex);
                continue;
            }

            frame = m_queue.dequeue();
        }
        m_fileMgr.writeFrame(frame);
    }
}

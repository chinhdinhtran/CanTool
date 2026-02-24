#ifndef CANLOGWRITER_H
#define CANLOGWRITER_H

#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

#include "can/log/ICanLogWriter.h"
#include "can/log/BlfFileManager.h"

class CanLogWriter : public QThread, public ICanLogWriter
{
    Q_OBJECT
public:
    explicit CanLogWriter(QObject* parent = nullptr);
    ~CanLogWriter() override;

    void startLogging() override;
    void stopLogging() override;

public slots:
    void onFrame(const CanRawFrame& frame) override;

protected:
    void run() override;

private:
    QQueue<CanRawFrame> m_queue;
    QMutex              m_mutex;
    QWaitCondition      m_cond;
    bool                m_running = false;

    BlfFileManager      m_fileMgr;
};
#endif // CANLOGWRITER_H

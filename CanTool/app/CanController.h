#ifndef CANCONTROLLER_H
#define CANCONTROLLER_H

#include <QObject>
#include <QString>

#include "can/dbc/DbcManager.h"
#include "can/log/CanLogWriter.h"


class SignalStore;
class CanReceiver;
class ICanDriver;
class CanLogWriter;

class CanController : public QObject
{
    Q_OBJECT
public:
    explicit CanController(SignalStore* store,
                           QObject* parent = nullptr);
    ~CanController();

    // ===== DBC =====
    bool loadDbc(const QString& path);
    void clearModel();
    const CanInformation& model() const;

    // ===== CAN lifecycle =====
    bool openCan();   // open Vector device
    void closeCan();

    void start();     // start RX

    bool isRunning() const;

Q_SIGNALS:
    // emitted after DBC load (for UI signal list)
    void dbcLoaded();

private:
    SignalStore* m_store = nullptr;

    // owned by controller
    DbcManager   m_dbcManager;
    ICanDriver*  m_driver = nullptr;
    CanReceiver* m_rx = nullptr;
    CanLogWriter*    m_logger = nullptr;
    bool m_canOpened = false;
};

#endif // CANCONTROLLER_H

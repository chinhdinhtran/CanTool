#ifndef DBCMANAGER_H
#define DBCMANAGER_H

#include <memory>
#include <QString>

#include "CanInformation.h"
#include "can/raw/CanRawFrame.h"
#include "can/decode/DecodedSignalValue.h"

namespace dbc {
class DbcFile;
class Network;
}

class DbcManager
{
public:
    bool loadFile(const QString& dbcPath);
    void clearModel();

    const CanInformation& canInformation() const;
    dbc::Network* network() const;
    void decodeFrame(const CanRawFrame&, QVector<DecodedSignalValue>& out);
private:
    void parse();

private:
    std::shared_ptr<dbc::DbcFile> m_dbcFile;
    CanInformation m_canInfo;
};

#endif // DBCMANAGER_H

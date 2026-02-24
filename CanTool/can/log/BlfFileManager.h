#ifndef BLFFILEMANAGER_H
#define BLFFILEMANAGER_H

#include <QString>
#include <QDateTime>
#include <windows.h>

#include "binlog.h"
#include "binlog_objects.h"
#include "can/raw/CanRawFrame.h"

class BlfFileManager
{
public:
    BlfFileManager();
    ~BlfFileManager();

    bool open();
    void close();

    bool writeFrame(const CanRawFrame& frame);
    bool isOpen() const;

private:
    void rotateIfNeeded(uint32_t objectSize);
    QString makeFileName() const;
    bool openNewFile();

private:
    BLHANDLE   m_handle = BLINVALID_HANDLE_VALUE;
    QDateTime  m_sessionTime;
    int        m_fileIndex = 1;
    uint64_t   m_currentSize = 0;

    static constexpr uint64_t MAX_FILE_SIZE = 5ULL * 1024 * 1024 * 1024; // 5GB
};

#endif // BLFFILEMANAGER_H

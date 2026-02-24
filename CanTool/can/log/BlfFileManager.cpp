#include "BlfFileManager.h"
#include <QDebug>
#include <cstring>

static SYSTEMTIME GetLocalSystemTime()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    return st;
}

BlfFileManager::BlfFileManager()
{
    m_sessionTime = QDateTime::currentDateTime();
}

BlfFileManager::~BlfFileManager()
{
    close();
}

bool BlfFileManager::open()
{
    m_fileIndex = 1;
    m_currentSize = 0;
    return openNewFile();
}

bool BlfFileManager::openNewFile()
{
    close();

    QString fileName = makeFileName();
    m_handle = BLCreateFile(fileName.toLocal8Bit().constData(), GENERIC_WRITE);

    if (m_handle == BLINVALID_HANDLE_VALUE) {
        qCritical() << "BLCreateFile failed:" << fileName;
        return false;
    }

    SYSTEMTIME st = GetLocalSystemTime();

    if (!BLSetApplication(m_handle, BL_APPID_CANCASEXLLOG, 1, 0, 1) ||
        !BLSetMeasurementStartTime(m_handle, &st) ||
        !BLSetWriteOptions(m_handle, 0, 0)) {
        qCritical() << "BLF header setup failed";
        close();
        return false;
    }

    VBLAppTrigger trigger{};
    memset(&trigger, 0, sizeof(trigger));

    trigger.mHeader.mBase.mSignature     = BL_OBJ_SIGNATURE;
    trigger.mHeader.mBase.mHeaderSize    = sizeof(trigger.mHeader);
    trigger.mHeader.mBase.mHeaderVersion = 1;
    trigger.mHeader.mBase.mObjectSize    = sizeof(VBLAppTrigger);
    trigger.mHeader.mBase.mObjectType    = BL_OBJ_TYPE_APP_TRIGGER;
    trigger.mHeader.mObjectFlags         = BL_OBJ_FLAG_TIME_ONE_NANS;
    trigger.mHeader.mObjectTimeStamp     = 0;

    BLWriteObject(m_handle, &trigger.mHeader.mBase);
    m_currentSize += trigger.mHeader.mBase.mObjectSize;

    qInfo() << "Opened BLF:" << fileName;
    return true;
}

void BlfFileManager::close()
{
    if (m_handle != BLINVALID_HANDLE_VALUE) {
        BLCloseHandle(m_handle);
        m_handle = BLINVALID_HANDLE_VALUE;
    }
}

bool BlfFileManager::isOpen() const
{
    return m_handle != BLINVALID_HANDLE_VALUE;
}

QString BlfFileManager::makeFileName() const
{
    return QString("CanTool_PWT_%1_%2_%3.blf")
    .arg(m_sessionTime.toString("yyyyMMdd"))
        .arg(m_sessionTime.toString("HHmmss"))
        .arg(m_fileIndex);
}

void BlfFileManager::rotateIfNeeded(uint32_t objectSize)
{
    if (m_currentSize + objectSize < MAX_FILE_SIZE)
        return;

    ++m_fileIndex;
    m_currentSize = 0;
    openNewFile();
}

bool BlfFileManager::writeFrame(const CanRawFrame& frame)
{
    if (!isOpen())
        return false;

    VBLCANMessage msg{};
    memset(&msg, 0, sizeof(msg));

    msg.mHeader.mBase.mSignature     = BL_OBJ_SIGNATURE;
    msg.mHeader.mBase.mHeaderSize    = sizeof(msg.mHeader);
    msg.mHeader.mBase.mHeaderVersion = 1;
    msg.mHeader.mBase.mObjectSize    = sizeof(VBLCANMessage);
    msg.mHeader.mBase.mObjectType    = BL_OBJ_TYPE_CAN_MESSAGE;
    msg.mHeader.mObjectFlags         = BL_OBJ_FLAG_TIME_ONE_NANS;

    msg.mHeader.mObjectTimeStamp = frame.timestamp;
    msg.mChannel = 1;
    msg.mID      = frame.id;
    msg.mDLC     = frame.dlc;
    msg.mFlags   = frame.isExtended ? CAN_MSG_FLAGS(1, 0)
                                  : CAN_MSG_FLAGS(0, 0);

    memcpy(msg.mData, frame.data, frame.dlc);

    rotateIfNeeded(msg.mHeader.mBase.mObjectSize);

    if (!BLWriteObject(m_handle, &msg.mHeader.mBase))
        return false;

    m_currentSize += msg.mHeader.mBase.mObjectSize;
    return true;
}

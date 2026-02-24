#ifndef DECODEDSIGNALVALUE_H
#define DECODEDSIGNALVALUE_H


#include <QString>

/**
 * @brief DecodedSignalValue
 * DTO: output of DBC decode, input of runtime store
 */
struct DecodedSignalValue
{
    QString signalId;     // unique id: "0x123.EngineSpeed"
    double physicalValue; // after factor + offset
    double timestamp;     // seconds (monotonic)
};


#endif // DECODEDSIGNALVALUE_H

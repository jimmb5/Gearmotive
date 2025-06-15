#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <QVariant>
#include <QString>

// Tämän enumin tulee vastata STM32 enumia.
enum class SensorType : quint8 {
    OIL_TEMPERATURE = 0x10,
    PRIMARY_AXLE_RPM = 0x20,
    SECONDARY_AXLE_RPM = 0x21,
    GEARBOX_TORQUE = 0x30,
    BRAKE_TORQUE = 0x31,
    AIR_TEMPERATURE = 0x40,
    UNKNOWN = 0xFF
};

// Säilöö datan
struct SensorData {
    SensorType type;
    QString name;
    QVariant value;
    QString unit;
};

#endif // SENSORDATA_H 
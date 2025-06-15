#include "Communication.h"
#include "Sensor.h"
#include <Arduino.h>

/**
 * @brief Muodostaa ja lähettää datapaketin sarjaportin yli.
 */
void sendSensorData(Sensor* sensor) {
    uint8_t startByte = 0xAA;
    SensorType type = sensor->getType();
    uint8_t len = sensor->getDataSize();
    uint8_t* data = sensor->getData();
    
    uint8_t checksum = 0;
    checksum ^= startByte;
    checksum ^= (uint8_t)type;
    checksum ^= len;
    for (int i = 0; i < len; ++i) {
        checksum ^= data[i];
    }

    Serial.write(startByte);
    Serial.write((uint8_t)type);
    Serial.write(len);
    Serial.write(data, len);
    Serial.write(checksum);
} 
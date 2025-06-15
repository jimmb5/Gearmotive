#include "Sensor.h"
#include <stdint.h>
/*Sensor definet tähän
Kuten
#define OilTempSensor xx
#define PrimaryAxleRPMSensor xx
#define SecondaryAxleRPMSensor xx
#define GearboxTorqueSensor xx
#define BrakeTorqueSensor xx
#define AirTempSensor xx
*/
#include <cstdlib>

// ------ OilTempSensor toteutus ------

void OilTempSensor::begin() {
    temperature = 60.0; // Aloituslämpötila
}

void OilTempSensor::read() {
    // Simuloidaan lämpötilan hidasta nousua
    temperature += 0.05;
    if (temperature > 120.0) {
        temperature = 60.0;
    }
    // Lue sensorin arvo ESIM. 12 bit ADC!!
    // uint16_t sensorValue = analogRead(OilTempSensor);
    // temperature = sensorValue * 3.3 / 4095 ;
}

uint8_t* OilTempSensor::getData() {
    return reinterpret_cast<uint8_t*>(&temperature);
}

uint8_t OilTempSensor::getDataSize() {
    return sizeof(temperature);
}

SensorType OilTempSensor::getType() {
    return OIL_TEMPERATURE;
}

// ------ PrimaryAxleRPMSensor toteutus ------

void PrimaryAxleRPMSensor::begin() {
    rpm = 800;
}

void PrimaryAxleRPMSensor::read() {
    // Simuloidaan kierrosluvun muutosta
    rpm = (rpm + 50) % 5000;
}

uint8_t* PrimaryAxleRPMSensor::getData() {
    return reinterpret_cast<uint8_t*>(&rpm);
}

uint8_t PrimaryAxleRPMSensor::getDataSize() {
    return sizeof(rpm);
}

SensorType PrimaryAxleRPMSensor::getType() {
    return PRIMARY_AXLE_RPM;
}

// ------ SecondaryAxleRPMSensor toteutus ------

void SecondaryAxleRPMSensor::begin() {
    rpm = 300;
}

void SecondaryAxleRPMSensor::read() {
    // Simuloidaan kierrosluvun muutosta
    rpm = (rpm + 25) % 3000;
}

uint8_t* SecondaryAxleRPMSensor::getData() {
    return reinterpret_cast<uint8_t*>(&rpm);
}

uint8_t SecondaryAxleRPMSensor::getDataSize() {
    return sizeof(rpm);
}

SensorType SecondaryAxleRPMSensor::getType() {
    return SECONDARY_AXLE_RPM;
}

// ------ GearboxTorqueSensor toteutus ------

void GearboxTorqueSensor::begin() {
    torque = 100.0;
}

void GearboxTorqueSensor::read() {
    // Simuloidaan vääntömomentin muutosta
    torque += 2.5;
    if (torque > 400.0) {
        torque = 100.0;
    }
}

uint8_t* GearboxTorqueSensor::getData() {
    return reinterpret_cast<uint8_t*>(&torque);
}

uint8_t GearboxTorqueSensor::getDataSize() {
    return sizeof(torque);
}

SensorType GearboxTorqueSensor::getType() {
    return GEARBOX_TORQUE;
}

// ------ BrakeTorqueSensor toteutus ------

void BrakeTorqueSensor::begin() {
    torque = 0.0;
}

void BrakeTorqueSensor::read() {
    // Simuloidaan jarrutusmomentin satunnaista piikkiä
    if ((rand() % 10) == 0) {
        torque = static_cast<float>(rand() % 500);
    } else {
        torque *= 0.8; // Vaimenee nopeasti
    }
}

uint8_t* BrakeTorqueSensor::getData() {
    return reinterpret_cast<uint8_t*>(&torque);
}

uint8_t BrakeTorqueSensor::getDataSize() {
    return sizeof(torque);
}

SensorType BrakeTorqueSensor::getType() {
    return BRAKE_TORQUE;
}

// ------ AirTempSensor toteutus ------

void AirTempSensor::begin() {
    temperature = 15.0;
}

void AirTempSensor::read() {
    // Simuloidaan pientä vaihtelua
    temperature += (static_cast<float>(rand()) / RAND_MAX - 0.5) * 0.2;
    if (temperature > 25.0 || temperature < 5.0) {
        temperature = 15.0;
    }
}

uint8_t* AirTempSensor::getData() {
    return reinterpret_cast<uint8_t*>(&temperature);
}

uint8_t AirTempSensor::getDataSize() {
    return sizeof(temperature);
}

SensorType AirTempSensor::getType() {
    return AIR_TEMPERATURE;
} 
#include <Arduino.h>
#include "Sensor.h"
#include "Communication.h"

// Asetukset
#define BAUD_RATE 115200 // QT:n päässä oltava myös 115200
#define SENSOR_COUNT 6 // Tämä pitää muistaa päivittää aina kun lisätään sensoreita
#define SEND_INTERVAL 1000 
#define ADC_RESOLUTION 12 // 12 bit ADC

Sensor* sensors[SENSOR_COUNT];

void setup() {
    Serial.begin(BAUD_RATE);
    analogReadResolution(ADC_RESOLUTION);

    //while (!Serial); // Odota, että sarjaportti on valmis

    // Alustetaan sensorit ja lisätään ne rekisteriin
    sensors[0] = new OilTempSensor();
    sensors[1] = new PrimaryAxleRPMSensor();
    sensors[2] = new SecondaryAxleRPMSensor();
    sensors[3] = new GearboxTorqueSensor();
    sensors[4] = new BrakeTorqueSensor();
    sensors[5] = new AirTempSensor();

    for (int i = 0; i < SENSOR_COUNT; ++i) {
        sensors[i]->begin();
    }
}

void loop() {
    // Käydään kaikki sensorit läpi, luetaan data ja lähetetään se
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        sensors[i]->read();
        sendSensorData(sensors[i]);
    }
    delay(SEND_INTERVAL); 
}

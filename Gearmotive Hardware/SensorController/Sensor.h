#pragma once

#include <stdint.h>

// Enumeraatio eri anturityypeille.
// Tämä auttaa QT-sovellusta tunnistamaan, mistä datalähteestä on kyse.
enum SensorType : uint8_t {
    OIL_TEMPERATURE      = 0x10,
    PRIMARY_AXLE_RPM     = 0x20,
    SECONDARY_AXLE_RPM   = 0x21,
    GEARBOX_TORQUE       = 0x30,
    BRAKE_TORQUE         = 0x31,
    AIR_TEMPERATURE      = 0x40,
};

/**
 * @brief Abstrakti kantaluokka kaikille järjestelmän antureille.
 * 
 * Tämä luokka määrittelee yhteisen rajapinnan, jota kaikkien
 * anturiluokkien on noudatettava. Tämä tekee antureiden käsittelystä
 * yhtenäistä ja uusien antureiden lisäämisestä helppoa
 */
class Sensor {
public:
    /**
     * @brief Alustaa anturin. Kutsutaan kerran setup()-funktiossa.
     */
    virtual void begin() = 0;

    /**
     * @brief Lukee uuden arvon anturilta. Kutsutaan säännöllisesti loop()-funktiossa. ts. tähän logiikka joka lukee anturin arvon
     */
    virtual void read() = 0;

    /**
     * @brief Palauttaa osoittimen anturin dataan.
     * @return uint8_t* Osoitin databufferiin.
     */
    virtual uint8_t* getData() = 0;

    /**
     * @brief Palauttaa anturin datan koon tavuina.
     * @return uint8_t Datan koko.
     */
    virtual uint8_t getDataSize() = 0;

    /**
     * @brief Palauttaa anturin tyypin SensorType-enumeraationa.
     * @return SensorType Anturin tyyppi.
     */
    virtual SensorType getType() = 0;
    
    /**
     * @brief Virtuaalinen purkaja on tärkeä kantaluokille.
     */
    virtual ~Sensor() {} 
}; 


// ------ Antureiden luokkamäärittelyt ------

/**
 * @brief Öljyn lämpötila-anturi.
 */
class OilTempSensor : public Sensor {
private:
    float temperature;
public:
    void begin() override;
    void read() override;
    uint8_t* getData() override;
    uint8_t getDataSize() override;
    SensorType getType() override;
};

/**
 * @brief Ensiöakselin kierrosnopeusanturi.
 */
class PrimaryAxleRPMSensor : public Sensor {
private:
    uint16_t rpm;
public:
    void begin() override;
    void read() override;
    uint8_t* getData() override;
    uint8_t getDataSize() override;
    SensorType getType() override;
};

/**
 * @brief Toisioakselin kierrosnopeusanturi.
 */
class SecondaryAxleRPMSensor : public Sensor {
private:
    uint16_t rpm;
public:
    void begin() override;
    void read() override;
    uint8_t* getData() override;
    uint8_t getDataSize() override;
    SensorType getType() override;
};

/**
 * @brief Vaihteiston vääntömomenttianturi.
 */
class GearboxTorqueSensor : public Sensor {
private:
    float torque;
public:
    void begin() override;
    void read() override;
    uint8_t* getData() override;
    uint8_t getDataSize() override;
    SensorType getType() override;
};

/**
 * @brief Jarrun vääntömomenttianturi.
 */
class BrakeTorqueSensor : public Sensor {
private:
    float torque;
public:
    void begin() override;
    void read() override;
    uint8_t* getData() override;
    uint8_t getDataSize() override;
    SensorType getType() override;
};

/**
 * @brief Ilman lämpötila-anturi.
 */
class AirTempSensor : public Sensor {
private:
    float temperature;
public:
    void begin() override;
    void read() override;
    uint8_t* getData() override;
    uint8_t getDataSize() override;
    SensorType getType() override;
}; 
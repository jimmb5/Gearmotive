#pragma once

class Sensor; // Eteenpäin suuntautuva viittaus (forward declaration)

/**
 * @brief Muodostaa ja lähettää datapaketin sarjaportin yli.
 * 
 * Paketin formaatti:
 * - 1 tavu: Aloitusmerkki (0xAA)
 * - 1 tavu: Anturin tyyppi (SensorType)
 * - 1 tavu: Datan pituus
 * - N tavua: Data
 * - 1 tavu: Tarkistussumma (XOR)
 * 
 * @param sensor Osoitin sensoriin, jonka data lähetetään.
 */
void sendSensorData(Sensor* sensor); 
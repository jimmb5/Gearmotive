import serial
import struct
import time

# Määritellään sarjaportin asetukset
# Huom: Vaihda 'COM3' oikeaan porttiin omalla koneellasi.
# Windowsilla se on yleensä muotoa 'COMx', Linuxilla '/dev/ttyACMx' tai '/dev/ttyUSBx'
SERIAL_PORT = 'COM4' 
BAUD_RATE = 115200

# Sensorien tyypit (vastaa Sensor.h-tiedostoa)
SENSOR_TYPES = {
    0x01: "DUMMY_TEMPERATURE",
    0x02: "DUMMY_HUMIDITY"
}

def parse_data(sensor_type, payload):
    """Jäsennä saapunut data sensorin tyypin perusteella."""
    if sensor_type == 0x01: # DUMMY_TEMPERATURE
        # 4 tavua, little-endian float
        value = struct.unpack('<f', payload)[0]
        return f"{value:.2f} C"
    elif sensor_type == 0x02: # DUMMY_HUMIDITY
        # 2 tavua, little-endian unsigned short (uint16_t)
        value = struct.unpack('<H', payload)[0]
        return f"{value / 10.0:.1f} %"
    else:
        return "Tuntematon datatyyppi"

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Yhdistetty porttiin {SERIAL_PORT} nopeudella {BAUD_RATE}")
    except serial.SerialException as e:
        print(f"Virhe avattaessa sarjaporttia {SERIAL_PORT}: {e}")
        print("Tarkista, että STM32-levy on kytketty ja olet valinnut oikean portin.")
        return

    while True:
        try:
            # 1. Etsi aloitusmerkki (0xAA)
            if ser.read(1) == b'\xaa':
                start_byte = 0xAA
                
                # 2. Lue paketin header (tyyppi ja pituus)
                header = ser.read(2)
                if len(header) < 2:
                    continue 
                
                sensor_type, data_len = struct.unpack('<BB', header)
                
                # 3. Lue data (payload)
                payload = ser.read(data_len)
                if len(payload) < data_len:
                    continue

                # 4. Lue tarkistussumma
                received_checksum_byte = ser.read(1)
                if not received_checksum_byte:
                    continue
                received_checksum = received_checksum_byte[0]

                # 5. Varmenna tarkistussumma
                calculated_checksum = 0
                calculated_checksum ^= start_byte
                calculated_checksum ^= sensor_type
                calculated_checksum ^= data_len
                for byte in payload:
                    calculated_checksum ^= byte
                
                if calculated_checksum == received_checksum:
                    sensor_name = SENSOR_TYPES.get(sensor_type, "Tuntematon")
                    parsed_value = parse_data(sensor_type, payload)
                    print(f"Vastaanotettu data: Anturi={sensor_name} (0x{sensor_type:02X}), Arvo={parsed_value}")
                else:
                    print(f"Virheellinen tarkistussumma! Vastaanotettu: {received_checksum}, Laskettu: {calculated_checksum}")

        except serial.SerialException as e:
            print(f"Sarjaporttivirhe: {e}")
            break
        except KeyboardInterrupt:
            print("Lopetetaan...")
            break
        except Exception as e:
            print(f"Odottamaton virhe: {e}")


if __name__ == "__main__":
    main() 
#include "datareceiver.h"

#include <QDebug>
#include <QDataStream>

DataReceiver::DataReceiver(QObject *parent)
    : QObject(parent), m_serialPort(new QSerialPort(this)), m_buffer()
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &DataReceiver::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &DataReceiver::handleError);
}

DataReceiver::~DataReceiver()
{
    disconnectFromPort();
}

bool DataReceiver::connectToPort(const QString &portName, qint32 baudRate)
{
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadOnly)) {
        qDebug() << "Yhdistetty porttiin" << portName;
        emit portConnected();
        return true;
    } else {
        qDebug() << "Virhe avattaessa porttia" << portName << ":" << m_serialPort->errorString();
        emit errorOccurred(m_serialPort->errorString());
        return false;
    }
}

void DataReceiver::disconnectFromPort()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        qDebug() << "Portti suljettu.";
        emit portDisconnected();
    }
}

void DataReceiver::handleReadyRead()
{
    m_buffer.append(m_serialPort->readAll());
    processBuffer();
}

void DataReceiver::processBuffer()
{
    while (m_buffer.size() >= 4) { // Minimikoko paketille: start(1) + type(1) + len(1) + checksum(1) = 4
        // 1. Etsi aloitusmerkki
        int startIndex = m_buffer.indexOf(START_BYTE);
        if (startIndex == -1) {
            // Ei aloitusmerkkiä, tyhjennä puskuri
            m_buffer.clear();
            return;
        }

        // Siirrä puskurin alku aloitusmerkkiin
        if (startIndex > 0) {
            qDebug() << "Ohitetaan" << startIndex << "tavua roskaa puskurin alusta.";
            m_buffer = m_buffer.mid(startIndex);
        }

        // Tarvitaanko lisää dataa headerille?
        if (m_buffer.size() < 3) { // start(1) + type(1) + len(1)
            return; // Odota lisää dataa
        }

        // 2. Lue header (tyyppi ja pituus)
        quint8 sensorType_raw = m_buffer.at(1);
        quint8 dataLen = m_buffer.at(2);

        // 3. Tarkista onko koko paketti saapunut
        int packetSize = 1 + 1 + 1 + dataLen + 1; // start + type + len + payload + checksum
        if (m_buffer.size() < packetSize) {
            return; // Odota lisää dataa
        }

        // Nyt meillä on koko paketti puskurissa
        QByteArray packet = m_buffer.left(packetSize);
        m_buffer.remove(0, packetSize);

        // 4. Lue payload ja tarkistussumma
        QByteArray payload = packet.mid(3, dataLen);
        quint8 receivedChecksum = static_cast<quint8>(packet.at(packetSize - 1));

        // 5. Varmenna tarkistussumma
        quint8 calculatedChecksum = 0;
        // XOR-lasku start-tavusta payloadin loppuun
        for (int i = 0; i < packetSize - 1; ++i) {
            calculatedChecksum ^= static_cast<quint8>(packet.at(i));
        }

        if (calculatedChecksum == receivedChecksum) {
            SensorType type = static_cast<SensorType>(sensorType_raw);
            SensorData parsed = parsePayload(type, payload);
            qDebug() << "Vastaanotettu data: Anturi=" << parsed.name << "Arvo=" << parsed.value.toString() << parsed.unit;
            emit newDataReceived(parsed);
        } else {
            qDebug() << "Virheellinen tarkistussumma! Vastaanotettu:" << receivedChecksum << "Laskettu:" << calculatedChecksum;
        }
    }
}

SensorData DataReceiver::parsePayload(SensorType type, const QByteArray &payload)
{
    SensorData data;
    data.type = type;

    switch (type) {
    case SensorType::OIL_TEMPERATURE: {
        if (payload.size() == 4) {
            float value;
            QDataStream stream(payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
            stream >> value;
            data.name = "Öljylämpötila";
            data.value = QVariant(QString::number(value, 'f', 1));
            data.unit = " °C";
        }
        break;
    }
    case SensorType::PRIMARY_AXLE_RPM: {
        if (payload.size() == 2) {
            quint16 rpm;
            QDataStream stream(payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream >> rpm;
            data.name = "Ensiöakseli";
            data.value = QVariant(QString::number(rpm, 'f', 1));
            data.unit = " rpm";
        }
        break;
    }
    case SensorType::SECONDARY_AXLE_RPM: {
        if (payload.size() == 2) {
            quint16 rpm;
            QDataStream stream(payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream >> rpm;
            data.name = "Toisioakseli";
            data.value = QVariant(QString::number(rpm, 'f', 1));
            data.unit = " rpm";
        }
        break;
    }
    case SensorType::GEARBOX_TORQUE: {
        if (payload.size() == 4) {
            float value;
            QDataStream stream(payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
            stream >> value;
            data.name = "Vaihteiston vääntö";
            data.value = QVariant(QString::number(value, 'f', 1));
            data.unit = " Nm";
        }
        break;
    }
    case SensorType::BRAKE_TORQUE: {
        if (payload.size() == 4) {
            float value;
            QDataStream stream(payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
            stream >> value;
            data.name = "Jarrun vääntö";
            data.value = QVariant(QString::number(value, 'f', 1));
            data.unit = " Nm";
        }
        break;
    }
    case SensorType::AIR_TEMPERATURE: {
        if (payload.size() == 4) {
            float value;
            QDataStream stream(payload);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
            stream >> value;
            data.name = "Ilman lämpötila";
            data.value = QVariant(QString::number(value, 'f', 1));
            data.unit = " °C";
        }
        break;
    }
    default:
        data.name = "Tuntematon";
        data.value = QVariant(payload.toHex());
        data.unit = "";
        break;
    }
    return data;
}

void DataReceiver::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        QString errorString = m_serialPort->errorString();
        qDebug() << "Sarjaporttivirhe:" << errorString;
        emit errorOccurred(errorString);
    }
}

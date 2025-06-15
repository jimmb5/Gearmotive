#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <QObject>
#include <QSerialPort>
#include "sensordata.h"

class DataReceiver : public QObject
{
    Q_OBJECT
public:
    explicit DataReceiver(QObject *parent = nullptr);
    ~DataReceiver();

    bool connectToPort(const QString &portName, qint32 baudRate);
    void disconnectFromPort();

signals:
    void newDataReceived(const SensorData &data);
    void errorOccurred(const QString &errorString);
    void portConnected();
    void portDisconnected();

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    void processBuffer();
    SensorData parsePayload(SensorType type, const QByteArray &payload);

    QSerialPort *m_serialPort;
    QByteArray m_buffer;

    const quint8 START_BYTE = 0xAA;
};

#endif // DATARECEIVER_H

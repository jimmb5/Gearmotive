#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "sensordata.h"

class DataLogger : public QObject
{
    Q_OBJECT
public:
    explicit DataLogger(QObject *parent = nullptr);
    ~DataLogger();

    bool isLogging() const;

public slots:
    bool startLogging(const QString &filePath);
    void stopLogging();
    void logData(const SensorData &data);

signals:
    void loggingStatusChanged(bool isActive, const QString &filePath);
    void errorOccurred(const QString &error);

private:
    QFile m_logFile;
    QTextStream m_logStream;
    bool m_isLogging = false;
};

#endif // DATALOGGER_H 
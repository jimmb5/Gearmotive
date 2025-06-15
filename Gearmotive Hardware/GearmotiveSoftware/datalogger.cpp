#include "datalogger.h"
#include <QDateTime>
#include <QDebug>

DataLogger::DataLogger(QObject *parent) : QObject(parent)
{
}

DataLogger::~DataLogger()
{
    stopLogging();
}

bool DataLogger::isLogging() const
{
    return m_isLogging;
}

bool DataLogger::startLogging(const QString &filePath)
{
    if (m_isLogging) {
        // Jos lokitus on jo päällä, lopetetaan se ensin.
        stopLogging();
    }

    m_logFile.setFileName(filePath);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qWarning() << "Could not open log file for writing:" << m_logFile.errorString();
        emit errorOccurred(QString("Could not open log file for writing: %1").arg(m_logFile.errorString()));
        return false;
    }

    m_logStream.setDevice(&m_logFile);

    // Kirjoitetaan otsikkorivi, jos tiedosto on tyhjä.
    if (m_logFile.size() == 0) {
        m_logStream << "timestamp,name,value,unit\n";
    }

    m_isLogging = true;
    qDebug() << "Logging started to" << filePath;
    emit loggingStatusChanged(true, filePath);
    return true;
}

void DataLogger::stopLogging()
{
    if (m_isLogging) {
        m_logFile.close();
        m_isLogging = false;
        qDebug() << "Logging stopped.";
        emit loggingStatusChanged(false, "");
    }
}

void DataLogger::logData(const SensorData &data)
{
    if (!m_isLogging || !m_logFile.isOpen()) {
        return;
    }

    // Muunnetaan arvo merkkijonoksi ilman yksikköä, koska yksikkö on omassa sarakkeessaan
    QString valueString = data.value.toString();
    
    // Poistetaan mahdollinen yksikkö value-kentästä, jos se on sinne vahingossa päätynyt
    // (esim. "25.5 C" -> "25.5")
    // Tämä on vain varmistus, parsePayloadin tulisi palauttaa pelkkä numero QVariantissa
    valueString.remove(data.unit.trimmed());
    valueString = valueString.trimmed();


    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    m_logStream << timestamp << ","
                << data.name << ","
                << valueString << ","
                << data.unit.trimmed() << "\n";

    // Varmistetaan, että data kirjoitetaan heti käyttöjärjestelmän puskuriin.
    // Tämä minimoi datan menetyksen ohjelman kaatuessa, koska data ei jää
    // sovelluksen omaan puskuriin.
    m_logStream.flush();
} 
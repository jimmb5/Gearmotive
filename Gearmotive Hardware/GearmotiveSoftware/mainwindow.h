#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "datareceiver.h"
#include "datalogger.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QListWidget>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionDisconnect_triggered();
    void on_actionConnect_triggered();
    void updateLoggingStatus(bool isActive, const QString &filePath);
    void openLogFile();
    void onTimeSliderChanged(int value);
    void startLogging();
    void stopLogging();
    void onSensorSelectionChanged();
    void onCursorPositionChanged(qreal x);

private:
    struct SensorChartData {
        QLineSeries* series;
        QString unit;
    };

    void showSerialPortList();
    void showBaudRateList();
    void clearChartData();

    Ui::MainWindow *ui;
    DataReceiver *receiver;
    DataLogger *logger;
    QString selectedPortName;
    qint32 selectedBaudRate;

    QLabel *loggingStatusLabel;

    QChart *m_chart;
    QGraphicsLineItem *m_cursorLine;
    QGraphicsTextItem *m_cursorTextItem;
    QMap<QString, SensorChartData> m_sensorDataMap;
    QDateTime m_firstTimestamp;
    QDateTime m_lastTimestamp;
};
#endif // MAINWINDOW_H

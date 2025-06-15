#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interactivechartview.h"
#include <QSerialPortInfo>
#include <QActionGroup>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QMenu>
#include <QMap>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QGraphicsLineItem>
#include <QPen>
#include <QSignalBlocker>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , receiver(new DataReceiver(this))
    , logger(new DataLogger(this))
    , m_chart(new QChart())
    , m_cursorLine(nullptr)
    , m_cursorTextItem(nullptr)
{
    ui->setupUi(this);
    ui->splitter->setSizes({200, 800});
    this->showMaximized();
    showSerialPortList();
    showBaudRateList();

    connect(ui->actionStartLogging, &QAction::triggered, this, &MainWindow::startLogging);
    connect(ui->actionStopLogging, &QAction::triggered, this, &MainWindow::stopLogging);
    connect(ui->actionOpenLogFile, &QAction::triggered, this, &MainWindow::openLogFile);
    connect(ui->openLogFileButton, &QPushButton::clicked, this, &MainWindow::openLogFile);
    connect(ui->timeSlider, &QSlider::valueChanged, this, &MainWindow::onTimeSliderChanged);
    connect(ui->sensorListWidget, &QListWidget::currentItemChanged, this, &MainWindow::onSensorSelectionChanged);

    connect(ui->chartView, &InteractiveChartView::cursorPositionChanged, this, &MainWindow::onCursorPositionChanged);

    connect(ui->chartView, &InteractiveChartView::viewChanged, this, [this](){
        if (ui->timeSlider->isEnabled()) {
            onTimeSliderChanged(ui->timeSlider->value());
        }
    });

    // Asetetaan status bar
    loggingStatusLabel = new QLabel(this);
    statusBar()->addPermanentWidget(loggingStatusLabel);
    updateLoggingStatus(false, "");
    ui->actionStopLogging->setEnabled(false); // Aluksi pois päältä

    connect(receiver, &DataReceiver::newDataReceived, this, [this](const SensorData &data){
        qDebug() << "vastaanotettu: " << data.name << data.value.toString();

        if (data.name == "Öljylämpötila") {
            ui->OilTemp->setText(data.value.toString() + data.unit);
        }
        else if(data.name == "Ilman lämpötila"){
            ui->AirTemp->setText(data.value.toString() + data.unit);
        }
        else if(data.name == "Ensiöakseli"){
            ui->PrimaryAxleRpm->setText(data.value.toString() + data.unit);
        }
        else if(data.name == "Toisioakseli"){
            ui->SecondaryAxleRpm->setText(data.value.toString() + data.unit);
        }
        else if(data.name == "Vaihteiston vääntö"){
            ui->GearboxTorque->setText(data.value.toString() + data.unit);
        }
        else if(data.name == "Jarrun vääntö"){
            ui->BrakeTorque->setText(data.value.toString() + data.unit);
        }
    });

    connect(receiver, &DataReceiver::newDataReceived, logger, &DataLogger::logData);

    connect(logger, &DataLogger::loggingStatusChanged, this, &MainWindow::updateLoggingStatus);
    connect(logger, &DataLogger::errorOccurred, this, [this](const QString &err){
        QMessageBox::critical(this, tr("Lokitusvirhe"), err);
    });

    connect(receiver, &DataReceiver::errorOccurred, this, [this](const QString &err){
        qDebug() << "Virhe MainWindowissa: " << err;
        QMessageBox::critical(this, tr("Virhe"), tr("Yhteyden muodostaminen epäonnistui: %1.").arg(err));
    });

    // Alustetaan kuvaaja
    ui->chartView->setChart(m_chart);
    m_chart->setTheme(QChart::ChartThemeDark);
    m_chart->setTitle("Anturidatan aikasarja");
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openLogFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Avaa lokitiedosto"),
                                                    QDir::homePath(),
                                                    tr("CSV-tiedostot (*.csv);;Kaikki tiedostot (*.*)"));

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Virhe"), tr("Tiedoston avaaminen epäonnistui."));
        return;
    }
    
    clearChartData();
    
    QTextStream in(&file);
    // Ohitetaan otsikkorivi
    if (!in.atEnd()) {
        in.readLine();
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');
        if (parts.size() == 4) {
            QDateTime timestamp = QDateTime::fromString(parts[0], Qt::ISODateWithMs);
            QString name = parts[1];
            double value = parts[2].toDouble();
            QString unit = parts[3].trimmed();

            if (!timestamp.isValid()) continue;

            if (m_firstTimestamp.isNull()) m_firstTimestamp = timestamp;
            m_lastTimestamp = timestamp;

            if (!m_sensorDataMap.contains(name)) {
                m_sensorDataMap[name].series = new QLineSeries();
                m_sensorDataMap[name].series->setName(name);
                m_sensorDataMap[name].unit = unit;
            }
            m_sensorDataMap[name].series->append(timestamp.toMSecsSinceEpoch(), value);
        }
    }
    file.close();

    if (m_sensorDataMap.isEmpty()) {
        QMessageBox::information(this, tr("Tyhjä"), tr("Lokitiedosto ei sisältänyt dataa."));
        return;
    }
    
    ui->sensorListWidget->addItems(m_sensorDataMap.keys());

    ui->timeSlider->setRange(0, 1000);
    ui->timeSlider->setValue(0);
    ui->timeSlider->setEnabled(true);

    ui->tabWidget->setCurrentWidget(ui->logViewerTab);

    if (ui->sensorListWidget->count() > 0) {
        ui->sensorListWidget->setCurrentRow(0);
    }
}

void MainWindow::startLogging()
{
    QString defaultPath = QDir::homePath() + "/datalog.csv";
    QString filePath = QFileDialog::getSaveFileName(this, tr("Tallenna lokitiedosto"),
                                                    defaultPath,
                                                    tr("CSV-tiedostot (*.csv);;Kaikki tiedostot (*.*)"));

    if (!filePath.isEmpty()) {
        logger->startLogging(filePath);
    }
}

void MainWindow::stopLogging()
{
    logger->stopLogging();
}

void MainWindow::updateLoggingStatus(bool isActive, const QString &filePath)
{
    ui->actionStartLogging->setEnabled(!isActive);
    ui->actionStopLogging->setEnabled(isActive);

    if (isActive) {
        loggingStatusLabel->setText(tr("Lokitus aktiivinen: %1").arg(QFileInfo(filePath).fileName()));
    } else {
        loggingStatusLabel->setText(tr("Lokitus ei ole aktiivinen"));
    }
}

void MainWindow::on_actionDisconnect_triggered()
{
    receiver->disconnectFromPort();
}

void MainWindow::on_actionConnect_triggered()
{
    if(selectedPortName.isEmpty())
    {
        qDebug() << "Port not selected";
        return;
    }

    receiver->connectToPort(selectedPortName, selectedBaudRate);
}

void MainWindow::showSerialPortList()
{
    QActionGroup *portGroup = new QActionGroup(this);
    portGroup->setExclusive(true);

    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        QAction *portAction = new QAction(info.portName(), this);
        portAction->setCheckable(true);
        ui->menuPort_COM->addAction(portAction);
        portGroup->addAction(portAction);

        connect(portAction, &QAction::triggered, this, [this, portAction](){
           selectedPortName = portAction->text();
        });
    }

    if(!ui->menuPort_COM->actions().isEmpty()){
        ui->menuPort_COM->actions().first()->setChecked(true);
        selectedPortName = ui->menuPort_COM->actions().first()->text();
    }
}

void MainWindow::showBaudRateList()
{
    QActionGroup *baudRateGroup = new QActionGroup(this);
    baudRateGroup->setExclusive(true);

    const QList<qint32> baudRates = {9600, 19200, 38400, 57600, 115200};

    for(qint32 baud : baudRates){
        QAction *baudRateAction = new QAction(QString::number(baud), this);
        baudRateAction->setCheckable(true);
        ui->menuBaud_rate->addAction(baudRateAction);
        baudRateGroup->addAction(baudRateAction);

        connect(baudRateAction, &QAction::triggered, this, [this, baud](){
           selectedBaudRate = baud;
        });
    }

    QList<QAction*> actions = ui->menuBaud_rate->actions();
    for(QAction *action : actions){
        if(action->text() == "115200"){
            action->setChecked(true);
            selectedBaudRate = 115200;
            break;
        }
    }
}

void MainWindow::onSensorSelectionChanged()
{
    QListWidgetItem *currentItem = ui->sensorListWidget->currentItem();
    if (!currentItem) {
        return;
    }
    QString name = currentItem->text();
    if (!m_sensorDataMap.contains(name)) {
        return;
    }

    // Poistetaan vanha sarja kaaviosta ilman että sitä tuhotaan,
    // jotta voimme käyttää sitä uudelleen. Kaavio luopuu omistajuudesta.
    for (auto s : m_chart->series()) {
        m_chart->removeSeries(s);
    }

    for (QAbstractAxis *axis : m_chart->axes()) {
        m_chart->removeAxis(axis);
        delete axis;
    }

    SensorChartData data = m_sensorDataMap[name];
    m_chart->addSeries(data.series); // Kaavio ottaa taas omistajuuden

    m_chart->setTitle(name);

    // Luodaan akselit
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm:ss.zzz");
    axisX->setTitleText("Aika");
    axisX->setRange(m_firstTimestamp, m_lastTimestamp);
    m_chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText(QString("Arvo (%1)").arg(data.unit));
    m_chart->addAxis(axisY, Qt::AlignLeft);

    // Kiinnitetään sarja akseleihin
    data.series->attachAxis(axisX);
    data.series->attachAxis(axisY);
    
    m_chart->legend()->hide();

    // Päivitetään heti myös sliderin arvot
    onTimeSliderChanged(ui->timeSlider->value());

    m_chart->axes(Qt::Vertical).first()->setTitleText("");
}

void MainWindow::onTimeSliderChanged(int value)
{
    if (m_firstTimestamp.isNull() || m_lastTimestamp.isNull() || m_firstTimestamp == m_lastTimestamp) {
        return;
    }

    qint64 firstMs = m_firstTimestamp.toMSecsSinceEpoch();
    qint64 lastMs = m_lastTimestamp.toMSecsSinceEpoch();
    qint64 totalDuration = lastMs - firstMs;

    if (totalDuration <= 0) return;

    qint64 timestampAtSlider = firstMs + (totalDuration * value) / ui->timeSlider->maximum();

    if (!m_cursorLine) {
        m_cursorLine = new QGraphicsLineItem(m_chart);
        m_cursorLine->setPen(QPen(Qt::red, 2));
        m_cursorLine->setZValue(5); // Varmistaa, että viiva on sarjojen päällä
    }
    if (!m_cursorTextItem) {
        m_cursorTextItem = new QGraphicsTextItem(m_chart);
        m_cursorTextItem->setZValue(10);
        m_cursorTextItem->setDefaultTextColor(Qt::white);
        m_cursorTextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }

    QLineSeries* selectedSeries = nullptr;
    QListWidgetItem* currentItem = ui->sensorListWidget->currentItem();
    QString unit;
    if (currentItem) {
        QString selectedSensorName = currentItem->text();
        if (m_sensorDataMap.contains(selectedSensorName)) {
            selectedSeries = m_sensorDataMap[selectedSensorName].series;
            unit = m_sensorDataMap[selectedSensorName].unit;
        }
    }

    qreal lineX = m_chart->mapToPosition(QPointF(timestampAtSlider, 0)).x();
    QRectF plotArea = m_chart->plotArea();
    m_cursorLine->setLine(lineX, plotArea.top(), lineX, plotArea.bottom());
    m_cursorLine->setVisible(true);

    if(selectedSeries && selectedSeries->count() > 0){
        QPointF closestPoint;
        qreal minDist = -1.0;

        const auto points = selectedSeries->pointsVector();
        for (const QPointF &point : points) {
            qreal dist = qAbs(point.x() - timestampAtSlider);
            if (minDist < 0 || dist < minDist) {
                minDist = dist;
                closestPoint = point;
            }
        }
        
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(qint64(closestPoint.x()));
        QString text = QString("Aika: %1\nArvo: %2 %3")
                           .arg(dt.toString("hh:mm:ss"))
                           .arg(closestPoint.y())
                           .arg(unit);

        m_cursorTextItem->setHtml(QString("<div style='background: rgba(30,30,30,0.8); color: white; padding: 4px; border-radius: 4px;'>%1</div>").arg(text.replace("\n", "<br/>")));

        QPointF textPos = m_chart->mapToPosition(closestPoint);

        const qreal margin = 10;
        textPos.setX(lineX + margin);

        if (textPos.x() + m_cursorTextItem->boundingRect().width() > plotArea.right()) {
            textPos.setX(lineX - m_cursorTextItem->boundingRect().width() - margin);
        }
        if (textPos.y() < plotArea.top()) {
            textPos.setY(plotArea.top());
        }
        if (textPos.y() + m_cursorTextItem->boundingRect().height() > plotArea.bottom()) {
            textPos.setY(plotArea.bottom() - m_cursorTextItem->boundingRect().height());
        }

        m_cursorTextItem->setPos(textPos);
        m_cursorTextItem->setVisible(true);

    } else {
        m_cursorTextItem->setVisible(false);
    }

    // Tyhjennetään vanhat arvot ja näytetään uudet kaikille sarjoille
    QLayoutItem* item;
    while ((item = ui->valuesLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    int row = 0;
    for (const auto& sensorName : m_sensorDataMap.keys()) {
        const auto& sensorData = m_sensorDataMap[sensorName];
        if (sensorData.series && sensorData.series->count() > 0) {
            QPointF closestPoint;
            qreal minDist = -1.0;

            const auto points = sensorData.series->pointsVector();
            for (const QPointF &point : points) {
                qreal dist = qAbs(point.x() - timestampAtSlider);
                if (minDist < 0 || dist < minDist) {
                    minDist = dist;
                    closestPoint = point;
                }
            }
            if (!closestPoint.isNull()) {
                QLabel *nameLabel = new QLabel(sensorData.series->name(), this);
                QLabel *valueLabel = new QLabel(QString::number(closestPoint.y(), 'f', 2) + " " + sensorData.unit, this);
                ui->valuesLayout->addWidget(nameLabel, row, 0);
                ui->valuesLayout->addWidget(valueLabel, row, 1);
                row++;
            }
        }
    }
}

void MainWindow::clearChartData()
{
    for (const auto& data : m_sensorDataMap) {
        m_chart->removeSeries(data.series);
        delete data.series;
    }
    m_sensorDataMap.clear();
    ui->sensorListWidget->clear();

    if (m_cursorLine) {
        m_chart->scene()->removeItem(m_cursorLine);
        delete m_cursorLine;
        m_cursorLine = nullptr;
    }
     if (m_cursorTextItem) {
        m_chart->scene()->removeItem(m_cursorTextItem);
        delete m_cursorTextItem;
        m_cursorTextItem = nullptr;
    }

    // Nollataan akselit (poistetaan vanhat ja luodaan uudet tarvittaessa)
    for (auto axis : m_chart->axes()) {
        m_chart->removeAxis(axis);
        delete axis;
    }

    m_firstTimestamp = QDateTime();
    m_lastTimestamp = QDateTime();

    ui->timeSlider->setEnabled(false);

    // Tyhjennetään arvot-laatikko
    QLayoutItem* item;
    while ((item = ui->valuesLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void MainWindow::onCursorPositionChanged(qreal x)
{
    if (m_firstTimestamp.isNull() || m_lastTimestamp.isNull() || m_firstTimestamp == m_lastTimestamp) {
        return;
    }

    qint64 firstMs = m_firstTimestamp.toMSecsSinceEpoch();
    qint64 lastMs = m_lastTimestamp.toMSecsSinceEpoch();
    qint64 totalDuration = lastMs - firstMs;

    if (totalDuration <= 0) return;

    qint64 currentPosMs = static_cast<qint64>(x);

    // Varmistetaan, että arvo on alueella
    if (currentPosMs < firstMs) currentPosMs = firstMs;
    if (currentPosMs > lastMs) currentPosMs = lastMs;

    double relativePos = static_cast<double>(currentPosMs - firstMs) / totalDuration;
    int sliderValue = static_cast<int>(relativePos * ui->timeSlider->maximum());

    // Estetään signaalien kiertäminen
    QSignalBlocker blocker(ui->timeSlider);
    ui->timeSlider->setValue(sliderValue);

    // Päivitetään myös kursoriviiva manuaalisesti
    onTimeSliderChanged(sliderValue);
}

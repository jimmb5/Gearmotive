#include "interactivechartview.h"
#include <QChart>

InteractiveChartView::InteractiveChartView(QWidget *parent)
    : QChartView(parent), m_isPanning(false), m_isDraggingRight(false)
{
    // Otetaan pois käytöstä kumilankazoomaus, jotta se ei häiritse panorointia.
    setRubberBand(QChartView::NoRubberBand);
    setCursor(Qt::OpenHandCursor);
}

void InteractiveChartView::wheelEvent(QWheelEvent *event)
{
    if (!chart()) {
        QChartView::wheelEvent(event);
        return;
    }

    qreal factor;
    if (event->angleDelta().y() > 0) {
        // Zoomaa sisään
        factor = 1.2;
    } else {
        // Zoomaa ulos
        factor = 1.0 / 1.2;
    }

    const QRectF plotArea = chart()->plotArea();
    const QPointF mousePos = event->position();

    if (plotArea.contains(mousePos)) {
        // Lasketaan uusi näkymäalue, joka on keskitetty hiiren kursoriin
        qreal newWidth = plotArea.width() / factor;
        qreal newHeight = plotArea.height() / factor;
        qreal left = mousePos.x() - (mousePos.x() - plotArea.left()) / factor;
        qreal top = mousePos.y() - (mousePos.y() - plotArea.top()) / factor;

        QRectF zoomRect(left, top, newWidth, newHeight);
        chart()->zoomIn(zoomRect);
    } else {
        chart()->zoom(factor); // Oletuszoom, jos kursori ei ole piirtoalueella
    }

    emit viewChanged();
    event->accept();
}

void InteractiveChartView::mousePressEvent(QMouseEvent *event)
{
    if (!chart()) {
        QChartView::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton && chart()->plotArea().contains(event->position())) {
        m_isPanning = true;
        m_lastPanPoint = event->position();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else if (event->button() == Qt::RightButton && chart()->plotArea().contains(event->position())) {
        m_isDraggingRight = true;
        // Käsittele heti painallus, jotta kursori siirtyy heti
        if (chart()) {
            QPointF scenePos = event->position();
            QPointF chartPoint = chart()->mapToValue(scenePos);
            emit cursorPositionChanged(chartPoint.x());
        }
        event->accept();
    } else {
        QChartView::mousePressEvent(event);
    }
}

void InteractiveChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!chart()) {
        QChartView::mouseMoveEvent(event);
        return;
    }

    if (m_isPanning) {
        QPointF delta = event->position() - m_lastPanPoint;
        // Vieritetään vastakkaiseen suuntaan kuin hiiren liike (vaaka),
        // mutta samaan suuntaan pystysuunnassa (käänteinen logiikka).
        chart()->scroll(-delta.x(), delta.y());
        m_lastPanPoint = event->position();

        emit viewChanged();
        event->accept();
    } else if (m_isDraggingRight) {
        if (chart()) {
            QPointF scenePos = event->position();
            // Varmistetaan, että liike on plot-alueen sisällä
            if (chart()->plotArea().contains(scenePos)) {
                QPointF chartPoint = chart()->mapToValue(scenePos);
                emit cursorPositionChanged(chartPoint.x());
            }
        }
        event->accept();
    } else {
        QChartView::mouseMoveEvent(event);
    }
}

void InteractiveChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!chart()) {
        QChartView::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
        setCursor(Qt::OpenHandCursor);
        event->accept();
    } else if (event->button() == Qt::RightButton) {
        m_isDraggingRight = false;
        event->accept();
    } else {
        QChartView::mouseReleaseEvent(event);
    }
} 
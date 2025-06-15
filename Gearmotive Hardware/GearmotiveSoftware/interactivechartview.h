#ifndef INTERACTIVECHARTVIEW_H
#define INTERACTIVECHARTVIEW_H

#include <QtCharts/QChartView>
#include <QMouseEvent>
#include <QWheelEvent>



/**
 * @class InteractiveChartView
 * @brief Tarjoaa interaktiivisen kaavionäkymän, joka tukee zoomausta ja panorointia.
 *
 * Tämä luokka laajentaa QChartView:tä lisäämällä tuen hiiren rullalla
 * zoomaamiseen ja hiiren vasemmalla painikkeella raahaamalla tapahtuvaan panorointiin.
 */
class InteractiveChartView : public QChartView
{
    Q_OBJECT

public:
    /**
     * @brief Luo uuden InteractiveChartView-olion.
     * @param parent Vanhempi-widget.
     */
    explicit InteractiveChartView(QWidget *parent = nullptr);

signals:
    /**
     * @brief Tämä signaali lähetetään, kun hiiren oikealla painikkeella vedetään kaaviota.
     * @param x Hiiren X-koordinaatti kaavion arvoissa.
     */
    void cursorPositionChanged(qreal x);

    /**
     * @brief Tämä signaali lähetetään, kun kaavionäkymä muuttuu (zoomataan tai panoroidaan).
     */
    void viewChanged();

protected:
    /**
     * @brief Käsittelee hiiren rullatapahtumat zoomatakseen kaaviota.
     * @param event Rullatapahtuma.
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief Käsittelee hiiren painallustapahtumat panoroinnin aloittamiseksi.
     * @param event Hiiritapahtuma.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Käsittelee hiiren liikutustapahtumat kaavion panoroimiseksi.
     * @param event Hiiritapahtuma.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Käsittelee hiiren vapautustapahtumat panoroinnin lopettamiseksi.
     * @param event Hiiritapahtuma.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isPanning;      ///< Kertoo, onko panorointi käynnissä.
    bool m_isDraggingRight; ///< Kertoo, onko hiiren oikean painikkeen veto käynnissä.
    QPointF m_lastPanPoint; ///< Tallentaa viimeisimmän hiiren sijainnin panoroinnin aikana.
};

#endif // INTERACTIVECHARTVIEW_H 

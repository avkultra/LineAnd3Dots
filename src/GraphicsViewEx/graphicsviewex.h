#ifndef GRAPHICSVIEWEX_H
#define GRAPHICSVIEWEX_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QColor>
#include <vector>

class QGraphicsLineItem;

class CGraphicsViewEx : public QGraphicsView
{
    Q_OBJECT

public:
    CGraphicsViewEx(QWidget* parent = nullptr);

    void drawGrid(bool bShow);
    void clearUserItems();
    void drawPoint(double x, double y, int id, bool isStart);
    void drawLineThrough(double px, double py, double angle, const QColor& color = Qt::red);
    void drawLineBetween(double x1, double y1, double x2, double y2, const QColor& color = Qt::red);

    void setGridVisible(bool visible);
    bool isGridVisible() const { return m_bGridVisible; }
    void toggleGrid();

public:
    QGraphicsScene* getScene() { return m_scene; }

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* m_scene;
    double m_zoom;
    bool m_panning;
    QPointF m_panStart;

    bool m_bGridVisible;                         // Флаг видимости сетки
    std::vector<QGraphicsLineItem*> m_gridLines; // Хранилище линий сетки
};

#endif // GRAPHICSVIEWEX_H

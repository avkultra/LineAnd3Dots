#include "graphicsviewex.h"
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <cmath>
#include <QWheelEvent>

CGraphicsViewEx::CGraphicsViewEx(QWidget* parent)
    : QGraphicsView(parent)
    , m_zoom(1.0)
    , m_panning(false)
    , m_bGridVisible(true)
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-1000, -1000, 2000, 2000);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::NoDrag);
    setBackgroundBrush(QBrush(QColor(248, 248, 255)));

    drawGrid(m_bGridVisible);  // Рисуем сетку согласно флагу
}

void CGraphicsViewEx::drawGrid(bool bShow)
{
    // Очищаем старую сетку
    for (size_t i = 0; i < m_gridLines.size(); ++i)
    {
        QGraphicsLineItem* line = m_gridLines[i];
        m_scene->removeItem(line);
        delete line;
    }
    m_gridLines.clear();

    // Если флаг false - выходим, сетка не рисуется
    if (!bShow)
        return;

    QPen lightPen(QColor(220, 220, 220), 1, Qt::DashLine);
    QPen darkPen(QColor(180, 180, 180), 1);
    QPen axisPen(QColor(100, 100, 100), 1.5);

    for (int i = -1000; i <= 1000; i += 50)
    {
        m_gridLines.push_back(m_scene->addLine(-1000, i, 1000, i, lightPen));
        m_gridLines.push_back(m_scene->addLine(i, -1000, i, 1000, lightPen));
    }

    for (int i = -1000; i <= 1000; i += 100)
    {
        m_gridLines.push_back(m_scene->addLine(-1000, i, 1000, i, darkPen));
        m_gridLines.push_back(m_scene->addLine(i, -1000, i, 1000, darkPen));
    }

    m_gridLines.push_back(m_scene->addLine(-1000, 0, 1000, 0, axisPen));
    m_gridLines.push_back(m_scene->addLine(0, -1000, 0, 1000, axisPen));
}

void CGraphicsViewEx::setGridVisible(bool visible)
{
    if (m_bGridVisible == visible)
        return;

    m_bGridVisible = visible;
    drawGrid(m_bGridVisible);
}

void CGraphicsViewEx::toggleGrid()
{
    setGridVisible(!m_bGridVisible);
}

void CGraphicsViewEx::clearUserItems()
{
    QList<QGraphicsItem*> items = m_scene->items();

    for (int i = 0; i < items.count(); ++i)
    {
        QGraphicsItem* item = items[i];

        // Проверяем, является ли элемент линией сетки
        bool isGridLine = false;
        for (size_t j = 0; j < m_gridLines.size(); ++j)
        {
            QGraphicsLineItem* gridLine = m_gridLines[j];
            if (gridLine == item)
            {
                isGridLine = true;
                break;
            }
        }

        if (isGridLine)
            continue;  // Пропускаем линии сетки

        // Удаляем эллипсы (точки) и текст (подписи)
        if (dynamic_cast<QGraphicsEllipseItem*>(item) ||
            dynamic_cast<QGraphicsTextItem*>(item))
        {
            m_scene->removeItem(item);
            delete item;
        }
        // Удаляем линии, которые не являются сеткой (найденная прямая)
        else if (dynamic_cast<QGraphicsLineItem*>(item))
        {
            m_scene->removeItem(item);
            delete item;
        }
    }
}

void CGraphicsViewEx::drawPoint(double x, double y, int id, bool isStart)
{
    QColor fillColor = isStart ? QColor(220, 50, 50) : QColor(50, 100, 220);
    QPen pen(Qt::black, 1.5);
    m_scene->addEllipse(x - 5, y - 5, 10, 10, pen, QBrush(fillColor));

    QGraphicsTextItem* label = m_scene->addText(QString::number(id));
    label->setDefaultTextColor(isStart ? QColor(200, 0, 0) : QColor(0, 0, 150));
    label->setPos(x + 7, y - 12);
    label->setFont(QFont("Arial", 10, QFont::Bold));

    if (isStart)
    {
        QPen markerPen(QColor(220, 50, 50), 2, Qt::DashLine);
        m_scene->addEllipse(x - 12, y - 12, 24, 24, markerPen, Qt::NoBrush);
    }
}

void CGraphicsViewEx::drawLineThrough(double px, double py, double angle, const QColor& color)
{
    double dx = cos(angle);
    double dy = sin(angle);

    double t_min = -2000, t_max = 2000;

    if (fabs(dx) > 1e-9)
    {
        t_min = qMax(t_min, (-1000 - px) / dx);
        t_max = qMin(t_max, (1000 - px) / dx);
    }

    if (fabs(dy) > 1e-9)
    {
        t_min = qMax(t_min, (-1000 - py) / dy);
        t_max = qMin(t_max, (1000 - py) / dy);
    }

    QPointF p1(px + t_min * dx, py + t_min * dy);
    QPointF p2(px + t_max * dx, py + t_max * dy);

    QPen pen(color, 2.5);
    pen.setStyle(Qt::SolidLine);
    m_scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), pen);
}

void CGraphicsViewEx::drawLineBetween(double x1, double y1, double x2, double y2, const QColor& color)
{
    QPen pen(color, 2);
    m_scene->addLine(x1, y1, x2, y2, pen);
}

void CGraphicsViewEx::wheelEvent(QWheelEvent* event)
{
	/*
    double factor = 1.1;
    if (event->angleDelta().y() > 0)
    {
        scale(factor, factor);
        m_zoom *= factor;
    }
    else
    {
        scale(1.0 / factor, 1.0 / factor);
        m_zoom /= factor;
    }
	*/
}

void CGraphicsViewEx::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton)
    {
        m_panning = true;
        m_panStart = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}

void CGraphicsViewEx::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning)
    {
        QPointF delta = mapToScene(event->pos()) - mapToScene(m_panStart.x(), m_panStart.y());
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_panStart = event->pos();
        event->accept();
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void CGraphicsViewEx::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton)
    {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

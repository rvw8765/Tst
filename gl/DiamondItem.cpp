#include "diamonditem.h"
#include <QPainter>

DiamondItem::DiamondItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

QRectF DiamondItem::boundingRect() const
{
    return QRectF(-50, -50, 100, 100);
}

void DiamondItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPolygonF diamond;
    diamond << QPointF(0, -50)    // верх
            << QPointF(50, 0)     // право
            << QPointF(0, 50)     // низ
            << QPointF(-50, 0);   // лево

    //painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(Qt::blue);
    painter->setPen(QPen(Qt::darkBlue, 2));
    painter->drawPolygon(diamond);
}


PixelItem::PixelItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

QRectF PixelItem::boundingRect() const
{
    return QRectF(-40, -40, 80, 80);
}

void PixelItem::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Явно выключаем антиальясинг для этого объекта.
    painter->setRenderHint(QPainter::Antialiasing, false);

    QColor red = Qt::red;
    red.setAlpha(155);
    painter->setBrush(red);
    painter->setPen(QPen(Qt::darkRed, 1));
    //painter->drawEllipse(QRectF(-40, -40, 80, 80));
    QPolygonF tri;
    tri << QPointF(-40, 40) << QPointF(40, 40) << QPointF(0, -40);
    painter->drawPolygon(tri);
}

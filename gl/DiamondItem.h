#ifndef DIAMONDITEM_H
#define DIAMONDITEM_H

#include <QGraphicsItem>

class DiamondItem : public QGraphicsItem
{
public:
    explicit DiamondItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

class PixelItem : public QGraphicsItem
{
public:
    explicit PixelItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

#endif // DIAMONDITEM_H

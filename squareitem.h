#ifndef SQUAREITEM_H
#define SQUAREITEM_H

#include "fortunestreetdata.h"
#include <QGraphicsItem>

class SquareItem : public QGraphicsItem {
public:
    SquareItem(const SquareData &dataValue, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    SquareData &getData();
    QPointF getSnapLocation(const QPointF &loc);
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
private:
    SquareData data;
    static void drawTextCentered(QPainter *painter, int x, int y, const QString &text);
};

#endif // SQUAREITEM_H

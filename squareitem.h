#ifndef SQUAREITEM_H
#define SQUAREITEM_H

#include "fortunestreetdata.h"
#include <QGraphicsObject>

class SquareItem : public QGraphicsObject {
    Q_OBJECT
public:
    SquareItem(const SquareData &dataValue, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    SquareData &getData();
private:
    SquareData data;
    static void drawTextCentered(QPainter *painter, int x, int y, const QString &text);
private slots:
    void changeX();
    void changeY();
};

#endif // SQUAREITEM_H

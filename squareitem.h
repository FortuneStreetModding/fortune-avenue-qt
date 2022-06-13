#ifndef SQUAREITEM_H
#define SQUAREITEM_H

#include "fortunestreetdata.h"
#include <QEvent>
#include <QGraphicsItem>

class SquareItem : public QGraphicsItem {
public:
    SquareItem(const SquareData &dataValue, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    SquareData &getData();
    const SquareData &getData() const;
    QPointF getSnapLocation(const QPointF &loc);
    /**
     * This should be called after the ID is changed
     */
    void updateZValueFromData();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
private:
    SquareData data;
    QPointF oldPoint;
    static void drawBackgroundedTextCentered(QPainter *painter, int x, int y, const QString &text, const QBrush &bgBrush);
    static void drawBackgroundedTextRightAligned(QPainter *painter, int x, int y, const QString &text, const QBrush &bgBrush);
    static void drawBackgroundedText(QPainter *painter, int x, int y, const QString &text, const QBrush &bgBrush);
};

#endif // SQUAREITEM_H

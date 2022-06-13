#ifndef FORTUNEAVENUEGRAPHICSSCENE_H
#define FORTUNEAVENUEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include "squareitem.h"

class FortuneAvenueGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    FortuneAvenueGraphicsScene(qreal x, qreal y, qreal w, qreal h, QObject *parent = nullptr);
    FortuneAvenueGraphicsScene(const QRectF &rect, QObject *parent = nullptr);
    FortuneAvenueGraphicsScene(QObject *parent = nullptr);
    ~FortuneAvenueGraphicsScene() override;
    QVector<SquareItem *> squareItems();
    int getSnapSize() const;
    void setSnapSize(int value);
    void setAxesVisible(bool visible);
    void clearSquares();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    QMap<int, QPointF> oldPositions;
    int snapSize = 1;
    QVector<QGraphicsItem *> axesItems;
    void initAxesItems();
};

class FASceneSquareMoveEvent : public QEvent {
public:
    static const Type TYPE;
    FASceneSquareMoveEvent(const QMap<int, QPointF> &oldPositions, QMap<int, QPointF> &newPositions);
    const QMap<int, QPointF> &getOldPositions() const;
    const QMap<int, QPointF> &getNewPositions() const;
private:
    QMap<int, QPointF> oldPositions;
    QMap<int, QPointF> newPositions;
};

#endif // FORTUNEAVENUEGRAPHICSSCENE_H

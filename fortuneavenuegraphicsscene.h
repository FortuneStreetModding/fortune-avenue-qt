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
private:
    int snapSize = 1;
    QVector<QGraphicsItem *> axesItems;
    void initAxesItems();
};

#endif // FORTUNEAVENUEGRAPHICSSCENE_H

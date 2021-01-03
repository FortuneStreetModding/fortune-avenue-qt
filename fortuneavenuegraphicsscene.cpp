#include "fortuneavenuegraphicsscene.h"

FortuneAvenueGraphicsScene::FortuneAvenueGraphicsScene(qreal x, qreal y, qreal w, qreal h, QObject *parent)
    : QGraphicsScene(x, y, w, h, parent) {}
FortuneAvenueGraphicsScene::FortuneAvenueGraphicsScene(const QRectF &rect, QObject *parent)
    : QGraphicsScene(rect, parent) {}
FortuneAvenueGraphicsScene::FortuneAvenueGraphicsScene(QObject *parent) : QGraphicsScene(parent) {}

FortuneAvenueGraphicsScene::~FortuneAvenueGraphicsScene() {
    clearSelection();
}

int FortuneAvenueGraphicsScene::getSnapSize() const { return snapSize; }
void FortuneAvenueGraphicsScene::setSnapSize(int value) { snapSize = value; }

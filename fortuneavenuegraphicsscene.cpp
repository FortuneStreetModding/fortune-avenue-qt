#include "fortuneavenuegraphicsscene.h"

#include "darkdetect.h"

FortuneAvenueGraphicsScene::FortuneAvenueGraphicsScene(qreal x, qreal y, qreal w, qreal h, QObject *parent)
    : QGraphicsScene(x, y, w, h, parent) { initAxesItems(); }
FortuneAvenueGraphicsScene::FortuneAvenueGraphicsScene(const QRectF &rect, QObject *parent)
    : QGraphicsScene(rect, parent) { initAxesItems(); }
FortuneAvenueGraphicsScene::FortuneAvenueGraphicsScene(QObject *parent) : QGraphicsScene(parent) {
    initAxesItems();
}

FortuneAvenueGraphicsScene::~FortuneAvenueGraphicsScene() {
    clearSelection();
}

int FortuneAvenueGraphicsScene::getSnapSize() const { return snapSize; }
void FortuneAvenueGraphicsScene::setSnapSize(int value) { snapSize = value; }

QVector<SquareItem *> FortuneAvenueGraphicsScene::squareItems() {
    auto itemsList = items(Qt::AscendingOrder);
    QVector<SquareItem *> result;
    for (int i=axesItems.size(); i<itemsList.size(); ++i) {
        result.append((SquareItem *) itemsList[i]);
    }
    return result;
}

void FortuneAvenueGraphicsScene::initAxesItems() {
    auto sceneRectVal = sceneRect();
    QColor color = isDarkMode() ? Qt::white : Qt::black;
    axesItems.append(addLine(sceneRectVal.left(), 32, sceneRectVal.right(), 32, QPen(color)));
    axesItems.append(addLine(32, sceneRectVal.top(), 32, sceneRectVal.bottom(), QPen(color)));
    setAxesVisible(false);
}

void FortuneAvenueGraphicsScene::setAxesVisible(bool visible) {
    for (auto axesItem: qAsConst(axesItems)) {
        axesItem->setVisible(visible);
    }
}

void FortuneAvenueGraphicsScene::clearSquares() {
    auto squareItemsVal = squareItems();
    for (auto squareItem: qAsConst(squareItemsVal)) {
        removeItem(squareItem);
        delete squareItem;
    }
}

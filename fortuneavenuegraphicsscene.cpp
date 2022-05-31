#include "fortuneavenuegraphicsscene.h"

#include "darkdetect.h"

#include <QGraphicsSceneMouseEvent>

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
    auto itemsList = items();
    QVector<SquareItem *> result;
    for (int i=0; i<itemsList.size(); ++i) {
        auto sqItem = dynamic_cast<SquareItem *>(itemsList[i]);
        if (sqItem) result.append(sqItem);
    }
    std::sort(result.begin(), result.end(), [](const SquareItem *A, const SquareItem *B) {
        return A->getData().id < B->getData().id;
    });
    return result;
}

void FortuneAvenueGraphicsScene::initAxesItems() {
    auto sceneRectVal = sceneRect();
    QColor color = isDarkMode() ? Qt::white : Qt::black;
    axesItems.append(addLine(sceneRectVal.left(), 32, sceneRectVal.right(), 32, QPen(color)));
    axesItems.append(addLine(32, sceneRectVal.top(), 32, sceneRectVal.bottom(), QPen(color)));
    for (auto &item: axesItems) item->setZValue(-1000000);
    setAxesVisible(true);
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


void FortuneAvenueGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted()) {
        auto selItems = selectedItems();
        if (!selItems.empty()) {
            oldPositions.clear();
            for (auto &item: selItems) {
                SquareItem *sqItem = (SquareItem *)item;
                oldPositions[sqItem->getData().id] = sqItem->pos();
            }
        }
    }
}

void FortuneAvenueGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (event->isAccepted()) {
        auto selItems = selectedItems();
        if (!selItems.empty()) {
            QMap<int, QPointF> newPositions;
            for (auto &item: selItems) {
                SquareItem *sqItem = (SquareItem *)item;
                newPositions[sqItem->getData().id] = sqItem->pos();
            }
            // we are only moving if selected items remain the same and positions are different
            if (oldPositions.keys() == newPositions.keys() && oldPositions != newPositions) {
                FASceneSquareMoveEvent evt(oldPositions, newPositions);
                QApplication::sendEvent(this, &evt);
            }
        }
    }
}

const QEvent::Type FASceneSquareMoveEvent::TYPE = (QEvent::Type)QEvent::registerEventType();

FASceneSquareMoveEvent::FASceneSquareMoveEvent(const QMap<int, QPointF> &oldPositions, QMap<int, QPointF> &newPositions)
    : QEvent(TYPE), oldPositions(oldPositions), newPositions(newPositions)
{
}

const QMap<int, QPointF> &FASceneSquareMoveEvent::getOldPositions() const
{
    return oldPositions;
}

const QMap<int, QPointF> &FASceneSquareMoveEvent::getNewPositions() const
{
    return newPositions;
}

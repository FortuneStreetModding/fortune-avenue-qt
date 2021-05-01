#include "squareitem.h"

#include <QApplication>
#include <QPainter>
#include <QMap>
#include <QtMath>
#include <QVector>
#include "darkdetect.h"
#include "fortuneavenuegraphicsscene.h"
#include "static_block.hpp"

QMap<SquareType, QString> typeToFile = {
    {Property, ":/squares/GroundProperty.png"},
    {Bank, ":/squares/GroundBank.png"},
    {VentureSquare, ":/squares/GroundVenture.png"},
    {SuitSquareSpade, ":/squares/GroundSuit01.png"},
    {SuitSquareHeart, ":/squares/GroundSuit02.png"},
    {SuitSquareDiamond, ":/squares/GroundSuit03.png"},
    {SuitSquareClub, ":/squares/GroundSuit04.png"},
    {ChangeOfSuitSquareSpade, ":/squares/GroundCSuit01.png"},
    {ChangeOfSuitSquareHeart, ":/squares/GroundCSuit02.png"},
    {ChangeOfSuitSquareDiamond, ":/squares/GroundCSuit03.png"},
    {ChangeOfSuitSquareClub, ":/squares/GroundCSuit04.png"},
    {TakeABreakSquare, ":/squares/GroundTakeABreak.png"},
    {BoonSquare, ":/squares/GroundBoon.png"},
    {BoomSquare, ":/squares/GroundBoom.png"},
    {StockBrokerSquare, ":/squares/GroundStockBroker.png"},
    {RollOnSquare, ":/squares/GroundRollOn.png"},
    {ArcadeSquare, ":/squares/GroundArcade.png"},
    {SwitchSquare, ":/squares/GroundSwitch.png"},
    {CannonSquare, ":/squares/GroundCannon.png"},
    {BackStreetSquareA, ":/squares/GroundWarpA.png"},
    {BackStreetSquareB, ":/squares/GroundWarpB.png"},
    {BackStreetSquareC, ":/squares/GroundWarpC.png"},
    {BackStreetSquareD, ":/squares/GroundWarpD.png"},
    {BackStreetSquareE, ":/squares/GroundWarpE.png"},
    {OneWayAlleyDoorA, ":/squares/GroundDoorBlue.png"},
    {OneWayAlleyDoorB, ":/squares/GroundDoorRed.png"},
    {OneWayAlleyDoorC, ":/squares/GroundDoorYellow.png"},
    {OneWayAlleyDoorD, ":/squares/GroundDoorGreen.png"},
    {LiftMagmaliceSquareStart, ":/squares/GroundLiftMagmaliceStart.png"},
    {MagmaliceSquare, ":/squares/GroundMagmalice.png"},
    {OneWayAlleySquare, ":/squares/GroundDoorMario.png"},
    {LiftSquareEnd, ":/squares/GroundLift.png"},
    {EventSquare, ":/squares/EventSquare.png"},
    {VacantPlot, ":/squares/GroundVacant.png"}
};

QVector<QColor> districtColors = {
    QColor("#FF0000"),
    QColor("#00C0FF"),
    QColor("#FFBB00"),
    QColor("#00FF00"),
    QColor("#0000FF"),
    QColor("#FFA0C0"),
    QColor("#8000A0"),
    QColor("#FFFF60"),
    QColor("#008000"),
    QColor("#C080FF"),
    QColor("#FF8040"),
    QColor("#FF50A0")
};

QFont valueFont("Lato");
QFont idFont("Lato");
QFont rentFont("Lato");

static_block {
    valueFont.setPixelSize(18);
    idFont.setPixelSize(10);
    rentFont.setPixelSize(12);
}

SquareItem::SquareItem(const SquareData &dataValue, QGraphicsItem *parent) : QGraphicsItem(parent), data(dataValue) {
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setPos(data.positionX, data.positionY);
    setFlag(ItemSendsGeometryChanges);
}

QRectF SquareItem::boundingRect() const {
    return QRectF(0, 0, 64, 64);
}

void SquareItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->drawImage(QPoint(0, 0), QImage(typeToFile.value(data.squareType, ":/squares/GroundDefault.png")));

    QPen pen(Qt::transparent, 2, Qt::SolidLine);
    pen.setJoinStyle(Qt::MiterJoin);
    if (isSelected()) {
        pen.setColor(isDarkMode() ? QColor("#eeeeee") : QColor("#111111"));
        painter->setPen(pen);
        painter->drawRect(0 - 2/2, 0 - 2/2, 64 + 2, 64 + 2);
    }
    if (data.squareType == Property || data.squareType == VacantPlot) {
        pen.setColor(districtColors.value(data.districtDestinationId, Qt::transparent));
    }
    painter->setPen(pen);

    painter->drawRect(0 + 2/2, 0 + 2/2, 64 - 2, 64 - 2);

    QColor backgroundPen(0,0,0,127);

    if (data.squareType == Property) {
        painter->setPen(Qt::white);
        painter->setFont(valueFont);
        drawBackgroundedTextCentered(painter, 32, 38, QString::number(data.value), backgroundPen);
        painter->setFont(rentFont);
        drawBackgroundedTextRightAligned(painter, 60, 60, QString::number(data.price), backgroundPen);
    }

    painter->setPen(Qt::white);
    painter->setFont(idFont);
    drawBackgroundedText(painter, 2, 12, QString::number(data.id), backgroundPen);
}

SquareData &SquareItem::getData() {
    return data;
}

void SquareItem::drawBackgroundedTextCentered(QPainter *painter, int x, int y, const QString &text, const QBrush &bgBrush) {
    auto boundingRect = painter->fontMetrics().boundingRect(text);
    drawBackgroundedText(painter, x - boundingRect.width()/2, y, text, bgBrush);
}

void SquareItem::drawBackgroundedTextRightAligned(QPainter *painter, int x, int y, const QString &text, const QBrush &bgBrush) {
    auto boundingRect = painter->fontMetrics().boundingRect(text);
    drawBackgroundedText(painter, x - boundingRect.width(), y, text, bgBrush);
}

void SquareItem::drawBackgroundedText(QPainter *painter, int x, int y, const QString &text, const QBrush &bgBrush) {
    int width = painter->fontMetrics().horizontalAdvance(text);
    int ascent = painter->fontMetrics().ascent();
    int height = painter->fontMetrics().height();
    painter->fillRect(x, y - ascent, width, height, bgBrush);
    painter->drawText(x, y, text);
}

QVariant SquareItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange) {
        QPointF newPos = value.toPointF();
        if (QApplication::mouseButtons() == Qt::LeftButton) {
            newPos = getSnapLocation(newPos);
        }
        data.positionX = newPos.x();
        data.positionY = newPos.y();
        return newPos;
    }
    return QGraphicsItem::itemChange(change, value);
}

QPointF SquareItem::getSnapLocation(const QPointF &loc) {
    FortuneAvenueGraphicsScene *fortuneScene = qobject_cast<FortuneAvenueGraphicsScene *>(scene());
    if (fortuneScene) {
        int gridSize = fortuneScene->getSnapSize();
        qreal xV = qRound(loc.x()/gridSize)*gridSize;
        qreal yV = qRound(loc.y()/gridSize)*gridSize;
        return QPointF(xV, yV);
    }
    return loc;
}

#include "squareitem.h"
#include <QPainter>
#include <QMap>
#include <QVector>

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

QFont valueFont("Lato", 18);
QFont idFont("Lato", 10);

SquareItem::SquareItem(const SquareData &dataValue, QGraphicsItem *parent) : QGraphicsObject(parent), data(dataValue) {
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setPos(data.positionX, data.positionY);
    connect(this, &SquareItem::xChanged, this, &SquareItem::changeX);
    connect(this, &SquareItem::yChanged, this, &SquareItem::changeY);
}

QRectF SquareItem::boundingRect() const {
    return QRectF(0, 0, 64, 64);
}

void SquareItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->drawImage(QPoint(0, 0), QImage(typeToFile.value(data.squareType, ":/squares/GroundDefault.png")));

    QPen pen(Qt::transparent, 2, Qt::SolidLine);
    pen.setJoinStyle(Qt::MiterJoin);
    if (isSelected()) {
        pen.setColor(QColor("#eeeeee"));
    } else if (data.squareType == Property || data.squareType == VacantPlot) {
        pen.setColor(districtColors.value(data.districtDestinationId, Qt::transparent));
    }
    painter->setPen(pen);

    painter->drawRect(0 + 2/2, 0 + 2/2, 64 - 2, 64 - 2);

    if (data.squareType == Property) {
        painter->setPen(Qt::white);
        painter->setFont(valueFont);
        drawTextCentered(painter, 32, 38, QString::number(data.value));
    }

    painter->setPen(Qt::white);
    painter->setFont(idFont);
    painter->drawText(2, 12, QString::number(data.id));
}

SquareData &SquareItem::getData() {
    return data;
}

void SquareItem::drawTextCentered(QPainter *painter, int x, int y, const QString &text) {
    auto boundingRect = painter->fontMetrics().boundingRect(text);
    painter->drawText(x - boundingRect.width()/2, y, text);
}

void SquareItem::changeX() {
    data.positionX = x();
};

void SquareItem::changeY() {
    data.positionY = y();
};

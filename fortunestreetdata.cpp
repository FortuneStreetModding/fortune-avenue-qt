#include "fortunestreetdata.h"
#include "orderedmap.h"
#include <QtMath>

//#include <QDebug>

QDataStream &operator>>(QDataStream &stream, WaypointData &data) {
    stream >> data.entryId;
    for (quint8 &dest: data.destinations) {
        stream >> dest;
    }
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const WaypointData &data) {
    stream << data.entryId;
    for (quint8 dest: data.destinations) {
        stream << dest;
    }
    return stream;
}

int SquareData::getValueFromShopModel(int shopModel) {
    return shopModel*10;
}

void SquareData::updateValueFromShopModel() {
    value = getValueFromShopModel(shopModel);
}

qreal SquareData::getYield() {
    if(value == 0) return 0;
    return (qreal) price / (qreal) value;
}

QDataStream &operator>>(QDataStream &stream, SquareData &data) {
    stream >> data.squareType;
    stream >> data.positionX >> data.positionY;
    stream >> data.unknown1;
    for (auto &waypoint: data.waypoints) {
        stream >> waypoint;
    }
    stream >> data.districtDestinationId;
    stream >> data.oneWayLift;
    stream >> data.value >> data.price;
    stream >> data.unknown2;
    stream >> data.shopModel;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const SquareData &data) {
    stream << data.squareType;
    stream << data.positionX << data.positionY;
    stream << data.unknown1;
    for (auto &waypoint: data.waypoints) {
        stream << waypoint;
    }
    stream << data.districtDestinationId;
    stream << data.oneWayLift;
    stream << data.value << data.price;
    stream << data.unknown2;
    stream << data.shopModel;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Header &data) {
    char header[4];
    stream.readRawData(header, 4);
    if (QByteArray(header, 4) != data.magicNumber) {
        stream.setStatus(QDataStream::ReadCorruptData);
    } else {
        stream >> data.headerSize;
    }
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const Header &data) {
    stream.writeRawData(data.magicNumber, 4);
    stream << data.headerSize;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, BoardInfo &data) {
    stream >> data.header;
    stream.skipRawData(8);
    stream >> data.initialCash;
    stream >> data.targetAmount;
    stream >> data.baseSalary >> data.salaryIncrement;
    stream >> data.maxDiceRoll;
    stream >> data.galaxyStatus;
    stream >> data.versionFlag;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const BoardInfo &data) {
    stream << Header(data.header.getMagicNumber(), BoardInfo::SIZE);
    stream << (quint64)0;
    stream << data.initialCash;
    stream << data.targetAmount;
    stream << data.baseSalary << data.salaryIncrement;
    stream << data.maxDiceRoll;
    stream << data.galaxyStatus;
    stream << data.versionFlag;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, BoardData &data) {
    quint16 squareCount;
    stream >> data.header;
    stream.skipRawData(4);
    stream >> squareCount;
    stream.skipRawData(2);
    for (quint16 i=0; i<squareCount; ++i) {
        SquareData square(i);
        stream >> square;
        data.squares.append(square);
    }
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const BoardData &data) {
    stream << Header(data.header.getMagicNumber(), BoardData::SIZE + SquareData::SIZE*data.squares.size());
    stream << (quint32)0;
    stream << (quint16)data.squares.size();
    stream << (quint16)0;
    for (const auto &square: data.squares) {
        stream << square;
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, BoardFile &data) {
    stream >> data.header;
    stream >> data.unknown;
    stream >> data.boardInfo;
    stream >> data.boardData;
    if (data.boardInfo.versionFlag >= 1) {
        data.readAutopathData(stream);
    }
    if (data.boardInfo.versionFlag >= 2) {
        stream >> data.boardInfo.autopathRange >> data.boardInfo.straightLineTolerance;
    }
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const BoardFile &data) {
    stream << Header(data.header.getMagicNumber(), BoardFile::SIZE + SquareData::SIZE*data.boardData.squares.size());
    stream << data.unknown;
    stream << data.boardInfo;
    stream << data.boardData;
    if (data.boardInfo.versionFlag >= 1) {
        data.writeAutopathData(stream);
    }
    if (data.boardInfo.versionFlag >= 2) {
        stream << data.boardInfo.autopathRange << data.boardInfo.straightLineTolerance;
    }
    return stream;
}

static_assert(sizeof(AutoPath::DIRECTIONS)/sizeof(AutoPath::Direction) <= 8, "too many directions to fit into a 64-bit integer");

void BoardFile::readAutopathData(QDataStream &stream) {
    for (auto &square: boardData.squares) {
        square.validDirections.clear();
        quint64 autopathFlags;
        stream >> autopathFlags;
        for (auto from: AutoPath::DIRECTIONS) {
            for (auto to: AutoPath::DIRECTIONS) {
                if (autopathFlags & ( Q_UINT64_C(0x1) << (from * 8 + to) )) {
                    square.validDirections.insert(from, to);
                }
            }
        }
    }
}

void BoardFile::writeAutopathData(QDataStream &stream) const {
    for (auto &square: boardData.squares) {
        quint64 autopathFlags = 0;
        for (auto it=square.validDirections.begin(); it!=square.validDirections.end(); ++it) {
            autopathFlags |= Q_UINT64_C(0x1) << (it.key()*8 + it.value());
        }
        stream << autopathFlags;
    }
}

bool BoardFile::operator==(const BoardFile &other) const {
    QByteArray thisArr, otherArr;
    QDataStream thisStream(&thisArr, QIODevice::WriteOnly), otherStream(&otherArr, QIODevice::WriteOnly);
    thisStream << (*this);
    otherStream << other;
    return thisArr == otherArr;
}

bool BoardFile::operator!=(const BoardFile &other) const {
    return !(*this == other);
}

OrderedMap<QString, SquareType> textToSquareTypes = {
    {"Property", Property},
    {"Bank", Bank},
    {"Venture", VentureSquare},
    {"Spade", SuitSquareSpade},
    {"Heart", SuitSquareHeart},
    {"Diamond", SuitSquareDiamond},
    {"Club", SuitSquareClub},
    {"Spade (Change-of-Suit)", ChangeOfSuitSquareSpade},
    {"Heart (Change-of-Suit)", ChangeOfSuitSquareHeart},
    {"Diamond (Change-of-Suit)", ChangeOfSuitSquareDiamond},
    {"Club (Change-of-Suit)", ChangeOfSuitSquareClub},
    {"Take-A-Break", TakeABreakSquare},
    {"Boon", BoonSquare},
    {"Boom", BoomSquare},
    {"Stockbroker", StockBrokerSquare},
    {"Roll On", RollOnSquare},
    {"Arcade", ArcadeSquare},
    {"Switch", SwitchSquare},
    {"Cannon", CannonSquare},
    {"Backstreet A", BackStreetSquareA},
    {"Backstreet B", BackStreetSquareB},
    {"Backstreet C", BackStreetSquareC},
    {"Backstreet D", BackStreetSquareD},
    {"Backstreet E", BackStreetSquareE},
    {"One-Way Alley Door A", OneWayAlleyDoorA},
    {"One-Way Alley Door B", OneWayAlleyDoorB},
    {"One-Way Alley Door C", OneWayAlleyDoorC},
    {"One-Way Alley Door D", OneWayAlleyDoorD},
    {"Lift/Magmalice Start", LiftMagmaliceSquareStart},
    {"Lift End", LiftSquareEnd},
    {"Magmalice", MagmaliceSquare},
    {"One-Way Alley End", OneWayAlleySquare},
    {"Event", EventSquare},
    {"Vacant Plot", VacantPlot}
};
OrderedMap<QString, quint8> textToShopTypes = {
    {"",0},
    {"Scrap-paper shop",5},
    {"Wool shop",6},
    {"Bottle store",7},
    {"Secondhand book shop",8},
    {"Scrap-metal supplier",9},
    {"Stationery shop",10},
    {"General store",11},
    {"Florist's",12},
    {"Ice-cream shop",13},
    {"Comic-book shop",14},
    {"Dairy",15},
    {"Doughnut shop",16},
    {"Pizza shack",17},
    {"Bakery",18},
    {"Grocery store",19},
    {"Pharmacy",20},
    {"Fish market",21},
    {"Toy shop",22},
    {"Bookshop",23},
    {"Cosmetics boutique",24},
    {"T-shirt shop",25},
    {"Fruit stall",26},
    {"Photography studio",27},
    {"Coffee shop",28},
    {"Butcher shop",29},
    {"Restaurant",30},
    {"Barbershop",31},
    {"Hat boutique",32},
    {"Hardware store",33},
    {"Gift shop",34},
    {"Launderette",35},
    {"Shoe shop",36},
    {"Clothing store",37},
    {"Optician's",38},
    {"Clockmaker's",39},
    {"Furniture shop",40},
    {"Sports shop",41},
    {"Locksmith's",42},
    {"Glassmaker's",43},
    {"Sushi restaurant",44},
    {"Art gallery",45},
    {"Leatherware boutique",46},
    {"Pet shop",47},
    {"Nail salon",48},
    {"Spice shop",49},
    {"Music shop",50},
    {"Surf shop",51},
    {"Boating shop",52},
    {"Cartographer's",53},
    {"Alloy rims shop",54},
    {"Fashion boutique",55},
    {"Waxworks",56},
    {"Lens shop",57},
    {"Kaleidoscope shop",58},
    {"Crystal ball shop",59},
    {"Gemstone supplier",61},
    {"Taxidermy studio",62},
    {"Antiques dealer's",65},
    {"Goldsmith's",68},
    {"Fossil shop",70},
    {"Music-box shop",72},
    {"Marionette workshop",75},
    {"Health shop",76},
    {"Organic food shop",80},
    {"Bridal boutique",81},
    {"Autograph shop",85},
    {"Meteorite shop",90},
    {"Department store",98}
};

QString squareTypeToText(SquareType type) {
    for (auto it = textToSquareTypes.begin(); it != textToSquareTypes.end(); ++it) {
        if (it.value() == type) {
            return it.key();
        }
    }
    return "";
}
SquareType textToSquareType(QString string) { return textToSquareTypes.value(string, Property); }
QList<QString> squareTexts() { return textToSquareTypes.keys(); }

QString shopTypeToText(quint8 shopType) {
    for (auto it = textToShopTypes.begin(); it != textToShopTypes.end(); ++it) {
        if (it.value() == shopType) {
            return it.key();
        }
    }
    return "Unused";
}
QString shopTypeToTextWithValue(quint8 shopType) {
    for (auto it = textToShopTypes.begin(); it != textToShopTypes.end(); ++it) {
        if (it.value() == shopType) {
            return QString("(%1 G) %2").arg(SquareData::getValueFromShopModel(shopType)).arg(it.key());;
        }
    }
    return "Unused";
}
quint8 textToShopType(QString string) {
    for (auto it = textToShopTypes.begin(); it != textToShopTypes.end(); ++it) {
        auto shopName = it.key();
        if(shopName.isEmpty())
            continue;
        auto shopType = it.value();
        if(string.contains(shopName))
            return shopType;
    }
    return 0;
}
QList<QString> shopTexts() { return textToShopTypes.keys(); }
QList<QString> shopTextsWithValues() {
    QList<QString> shopTexts;
    for (auto it = textToShopTypes.begin(); it != textToShopTypes.end(); ++it) {
        auto shopType = it.value();
        if(it.key().isEmpty())
            shopTexts << it.key();
        else
            shopTexts << QString("(%1 G) %2").arg(SquareData::getValueFromShopModel(shopType)).arg(it.key());
    }
    return shopTexts;
}

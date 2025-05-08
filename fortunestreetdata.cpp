#include "fortunestreetdata.h"
#include "orderedmap.h"
#include "qdebug.h"
#include <QtMath>
#include <QSet>
#include <QIODevice>

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

SquareData &SquareData::syncForMultiState(const SquareData &other)
{
    squareType = other.squareType;
    if (squareType == Property || squareType == SwitchSquare) {
        districtDestinationId = other.districtDestinationId;
    }
    oneWayLift = other.oneWayLift;
    value = other.value;
    price = other.price;
    shopModel = other.shopModel;
    return *this;
}

bool SquareData::operator==(const SquareData &other) const
{
    QByteArray arr0, arr1;
    QDataStream ds0(&arr0, QIODevice::WriteOnly), ds1(&arr1, QIODevice::WriteOnly);
    ds0 << *this;
    ds1 << other;
    return arr0 == arr1 && validDirections == other.validDirections;
}

bool SquareData::operator!=(const SquareData &other) const {
    return !(*this == other);
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
    if (data.boardInfo.versionFlag >= 3) {
        stream >> data.boardInfo.useAdvancedAutoPath;
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
    if (data.boardInfo.versionFlag >= 3) {
        stream << data.boardInfo.useAdvancedAutoPath;
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

void BoardFile::verify(QStringList &errors, QStringList &warnings) {
    QVector<int> districtCount(12);
    int highestDistrict = -1;

    if (boardData.squares.size() < 3) {
        errors << FortuneStreetData::tr("Board must have at least 3 squares.");
    }
    if (boardData.squares.size() > 86) {
        errors << FortuneStreetData::tr("Board must have a maximum of 86 squares.");
    }
    if (boardInfo.maxDiceRoll < 1 || boardInfo.maxDiceRoll > 9) {
        errors << FortuneStreetData::tr("Maximum dice roll must be between 1 and 9 inclusive.");
    }
    for (auto &square: boardData.squares) {
        if (square.squareType == Property || square.squareType == VacantPlot) {
            if (square.districtDestinationId >= 12) {
                warnings << QString(FortuneStreetData::tr("Square %1 has district value %2. Maximum is 11.")).arg(square.id).arg(square.districtDestinationId);
            } else {
                ++districtCount[square.districtDestinationId];
                highestDistrict = qMax(highestDistrict, (int)square.districtDestinationId);
            }
        }
        // ignore waypoints for one-way alleys
        if (square.squareType == OneWayAlleyDoorA
                || square.squareType == OneWayAlleyDoorB
                || square.squareType == OneWayAlleyDoorC
                || square.squareType == OneWayAlleyDoorD
                || square.squareType == OneWayAlleySquare) {
            continue;
        }

        // ensure the ID set on a Switch Square is not 94 or higher
        if (square.squareType == SwitchSquare){
            if(square.districtDestinationId > 93){
                errors << FortuneStreetData::tr("The Board State ID of all Switch Squares must be 93 or lower.");
            }
        }

        // ensure Lift/Magmalice Square is connected to a Lift End or Magmalice square
        if (square.squareType == LiftMagmaliceSquareStart){
            // if it doesn't exist
            if(square.districtDestinationId > boardData.squares.size()){
                errors << QString(FortuneStreetData::tr("The destination square of Lift/Magmalice Start square with ID %1 was not found.")).arg(square.id);
            } else {
                SquareData &destSquare = boardData.squares[square.districtDestinationId];

                // if the destination square is not a Magmalice square
                if(destSquare.squareType == MagmaliceSquare || destSquare.squareType == LiftSquareEnd){
                    // do nothing
                } else {
                    errors << QString(FortuneStreetData::tr("The destination of Lift/Magmalice Start square with ID %1 is not a Magmalice square or a Lift End square.")).arg(square.id);
                }
            }
        }

        // ensure Magmalice Square or Lift End is connected to a Lift/Magmalice Start square
        if (square.squareType == MagmaliceSquare || square.squareType == LiftSquareEnd){
            // if it doesn't exist
            if(square.districtDestinationId > boardData.squares.size()){
                errors << QString(FortuneStreetData::tr("The destination of Magmalice square with ID %1 was not found.")).arg(square.id);
            } else {
                SquareData &destSquare = boardData.squares[square.districtDestinationId];
                // if the destination square is not a Lift/Magmalice Start square
                if(destSquare.squareType != LiftMagmaliceSquareStart){
                    errors << QString(FortuneStreetData::tr("The destination of Magmalice square with ID %1 is not a Lift/Magmalice Start square.")).arg(square.id);
                }
            }
        }

        // if all of a square's waypoints have undefined Entry IDs
        auto waypoints = square.waypoints;
        if(waypoints[0].entryId == 255 && waypoints[1].entryId == 255 && waypoints[2].entryId == 255 && waypoints[3].entryId == 255){
            warnings << QString(FortuneStreetData::tr("All of the waypoint entry IDs for Square ID %1 are undefined.")).arg(square.id);
        }

        QSet<quint8> destinations;
        for (int i=0; i<4; ++i) {
            if (square.waypoints[i].entryId > boardData.squares.size()) {
                if (square.waypoints[i].entryId != 255) {
                    errors << QString(FortuneStreetData::tr("Starting square of Waypoint %1 of Square %2 is Square %3 which does not exist"))
                              .arg(i+1).arg(square.id).arg(square.waypoints[i].entryId);
                } else {
                    // Since this waypoint has an entry point of 255, we ignore it entirely.
                    continue;
                }
            } else {
                auto &otherSquare = boardData.squares[square.waypoints[i].entryId];
                if (qAbs(square.positionX - otherSquare.positionX) > 96
                        || qAbs(square.positionY - otherSquare.positionY) > 96) {
                    warnings << QString(FortuneStreetData::tr("Starting square of Waypoint %1 of Square %2 is Square %3 which is too far"))
                              .arg(i+1).arg(square.id).arg(square.waypoints[i].entryId);
                }
            }
            if(square.squareType == LiftMagmaliceSquareStart || square.squareType == MagmaliceSquare || square.squareType == LiftSquareEnd){
                // if the square is one of these types, don't perform the next check, as these squares can actually work this way.
            } else {
                // if it's not a Lift/Magmalice square, and its waypoint's entry ID is defined, but all destinations are undefined
                if(square.waypoints[i].entryId != 255 &&
                    square.waypoints[i].destinations[0] == 255 &&
                    square.waypoints[i].destinations[1] == 255 &&
                    square.waypoints[i].destinations[2] == 255 &&
                    square.waypoints[i].destinations[3] == 255){

                    warnings << QString(FortuneStreetData::tr("All destinations for waypoint %1 of Square ID %2 are undefined.")).arg(i).arg(square.id);
                }
            }

            for (int j=0; j<3; ++j) {
                auto dest = square.waypoints[i].destinations[j];
                destinations.insert(dest);

                if (dest > boardData.squares.size()) {
                    if (dest != 255) {
                        errors << QString(FortuneStreetData::tr("Destination square #%4 of Waypoint %1 of Square %2 is Square %3 which does not exist"))
                                  .arg(i+1).arg(square.id).arg(dest).arg(j+1);
                    }
                } else {
                    auto &otherSquare = boardData.squares[dest];
                    if (qAbs(square.positionX - otherSquare.positionX) > 96
                            || qAbs(square.positionY - otherSquare.positionY) > 96) {
                        warnings << QString(FortuneStreetData::tr("Destination square #%4 of Waypoint %1 of Square %2 is Square %3 which is too far"))
                                  .arg(i+1).arg(square.id).arg(dest).arg(j+1);
                    }
                    if (otherSquare.squareType == OneWayAlleySquare &&
                            square.squareType != OneWayAlleyDoorA &&
                            square.squareType != OneWayAlleyDoorB &&
                            square.squareType != OneWayAlleyDoorC &&
                            square.squareType != OneWayAlleyDoorD) {
                        warnings << QString(FortuneStreetData::tr("Destination square #%4 of Waypoint %1 of Square %2 is Square %3 which is a One Way Alley Square"))
                                  .arg(i+1).arg(square.id).arg(dest).arg(j+1);
                    }
                }
            }
        }

        destinations.remove(255);
        if (destinations.size() > 4) {
            errors << QString(FortuneStreetData::tr("Square %1 has %2 destinations, which is more than the maximum of 4."))
                      .arg(square.id).arg(destinations.size());
        }
    }

    for (int i=0; i<=highestDistrict; ++i) {
        if (districtCount[i] == 0) {
            errors << QString(FortuneStreetData::tr("Did you skip District %1 when assigning districts?")).arg(i);
        } else if (districtCount[i] > 7) {
            errors << QString(FortuneStreetData::tr("District %1 has %2 shops which is more than the maximum of 7")).arg(i).arg(districtCount[i]);
        }
    }
}

OrderedMap<QString, SquareType> textToSquareTypes = {
    {SquareTypes::tr("Property"), Property},
    {SquareTypes::tr("Bank"), Bank},
    {SquareTypes::tr("Venture"), VentureSquare},
    {SquareTypes::tr("Spade"), SuitSquareSpade},
    {SquareTypes::tr("Heart"), SuitSquareHeart},
    {SquareTypes::tr("Diamond"), SuitSquareDiamond},
    {SquareTypes::tr("Club"), SuitSquareClub},
    {SquareTypes::tr("Spade (Change-of-Suit)"), ChangeOfSuitSquareSpade},
    {SquareTypes::tr("Heart (Change-of-Suit)"), ChangeOfSuitSquareHeart},
    {SquareTypes::tr("Diamond (Change-of-Suit)"), ChangeOfSuitSquareDiamond},
    {SquareTypes::tr("Club (Change-of-Suit)"), ChangeOfSuitSquareClub},
    {SquareTypes::tr("Take-A-Break"), TakeABreakSquare},
    {SquareTypes::tr("Boon"), BoonSquare},
    {SquareTypes::tr("Boom"), BoomSquare},
    {SquareTypes::tr("Stockbroker"), StockBrokerSquare},
    {SquareTypes::tr("Roll On"), RollOnSquare},
    {SquareTypes::tr("Arcade"), ArcadeSquare},
    {SquareTypes::tr("Switch"), SwitchSquare},
    {SquareTypes::tr("Cannon"), CannonSquare},
    {SquareTypes::tr("Backstreet A"), BackStreetSquareA},
    {SquareTypes::tr("Backstreet B"), BackStreetSquareB},
    {SquareTypes::tr("Backstreet C"), BackStreetSquareC},
    {SquareTypes::tr("Backstreet D"), BackStreetSquareD},
    {SquareTypes::tr("Backstreet E"), BackStreetSquareE},
    {SquareTypes::tr("One-Way Alley Door A"), OneWayAlleyDoorA},
    {SquareTypes::tr("One-Way Alley Door B"), OneWayAlleyDoorB},
    {SquareTypes::tr("One-Way Alley Door C"), OneWayAlleyDoorC},
    {SquareTypes::tr("One-Way Alley Door D"), OneWayAlleyDoorD},
    {SquareTypes::tr("Lift/Magmalice Start"), LiftMagmaliceSquareStart},
    {SquareTypes::tr("Lift End"), LiftSquareEnd},
    {SquareTypes::tr("Magmalice"), MagmaliceSquare},
    {SquareTypes::tr("One-Way Alley End"), OneWayAlleySquare},
    {SquareTypes::tr("Event"), EventSquare},
    {SquareTypes::tr("Vacant Plot"), VacantPlot}
};
OrderedMap<QString, quint8> textToShopTypes = {
    {"",0},
    {ShopNames::tr("Scrap-paper shop"),5},
    {ShopNames::tr("Wool shop"),6},
    {ShopNames::tr("Bottle store"),7},
    {ShopNames::tr("Secondhand book shop"),8},
    {ShopNames::tr("Scrap-metal supplier"),9},
    {ShopNames::tr("Stationery shop"),10},
    {ShopNames::tr("General store"),11},
    {ShopNames::tr("Florist's"),12},
    {ShopNames::tr("Ice-cream shop"),13},
    {ShopNames::tr("Comic-book shop"),14},
    {ShopNames::tr("Dairy"),15},
    {ShopNames::tr("Doughnut shop"),16},
    {ShopNames::tr("Pizza shack"),17},
    {ShopNames::tr("Bakery"),18},
    {ShopNames::tr("Grocery store"),19},
    {ShopNames::tr("Pharmacy"),20},
    {ShopNames::tr("Fish market"),21},
    {ShopNames::tr("Toy shop"),22},
    {ShopNames::tr("Bookshop"),23},
    {ShopNames::tr("Cosmetics boutique"),24},
    {ShopNames::tr("T-shirt shop"),25},
    {ShopNames::tr("Fruit stall"),26},
    {ShopNames::tr("Photography studio"),27},
    {ShopNames::tr("Coffee shop"),28},
    {ShopNames::tr("Butcher shop"),29},
    {ShopNames::tr("Restaurant"),30},
    {ShopNames::tr("Barbershop"),31},
    {ShopNames::tr("Hat boutique"),32},
    {ShopNames::tr("Hardware store"),33},
    {ShopNames::tr("Gift shop"),34},
    {ShopNames::tr("Launderette"),35},
    {ShopNames::tr("Shoe shop"),36},
    {ShopNames::tr("Clothing store"),37},
    {ShopNames::tr("Optician's"),38},
    {ShopNames::tr("Clockmaker's"),39},
    {ShopNames::tr("Furniture shop"),40},
    {ShopNames::tr("Sports shop"),41},
    {ShopNames::tr("Locksmith's"),42},
    {ShopNames::tr("Glassmaker's"),43},
    {ShopNames::tr("Sushi restaurant"),44},
    {ShopNames::tr("Art gallery"),45},
    {ShopNames::tr("Leatherware boutique"),46},
    {ShopNames::tr("Pet shop"),47},
    {ShopNames::tr("Nail salon"),48},
    {ShopNames::tr("Spice shop"),49},
    {ShopNames::tr("Music shop"),50},
    {ShopNames::tr("Surf shop"),51},
    {ShopNames::tr("Boating shop"),52},
    {ShopNames::tr("Cartographer's"),53},
    {ShopNames::tr("Alloy rims shop"),54},
    {ShopNames::tr("Fashion boutique"),55},
    {ShopNames::tr("Waxworks"),56},
    {ShopNames::tr("Lens shop"),57},
    {ShopNames::tr("Kaleidoscope shop"),58},
    {ShopNames::tr("Crystal ball shop"),59},
    {ShopNames::tr("Gemstone supplier"),61},
    {ShopNames::tr("Taxidermy studio"),62},
    {ShopNames::tr("Antiques dealer's"),65},
    {ShopNames::tr("Goldsmith's"),68},
    {ShopNames::tr("Fossil shop"),70},
    {ShopNames::tr("Music-box shop"),72},
    {ShopNames::tr("Marionette workshop"),75},
    {ShopNames::tr("Health shop"),76},
    {ShopNames::tr("Organic food shop"),80},
    {ShopNames::tr("Bridal boutique"),81},
    {ShopNames::tr("Autograph shop"),85},
    {ShopNames::tr("Meteorite shop"),90},
    {ShopNames::tr("Department store"),98}
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

bool BoardInfo::operator==(const BoardInfo &other) const
{
    return initialCash == other.initialCash
            && baseSalary == other.baseSalary
            && targetAmount == other.targetAmount
            && salaryIncrement == other.salaryIncrement
            && maxDiceRoll == other.maxDiceRoll
            && galaxyStatus == other.galaxyStatus
            && versionFlag == other.versionFlag
            && autopathRange == other.autopathRange
            && straightLineTolerance == other.straightLineTolerance
            && useAdvancedAutoPath == other.useAdvancedAutoPath;
}

bool BoardInfo::operator!=(const BoardInfo &other) const {
    return !(*this == other);
}

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
    {FortuneStreetData::tr("Property"), Property},
    {FortuneStreetData::tr("Bank"), Bank},
    {FortuneStreetData::tr("Venture"), VentureSquare},
    {FortuneStreetData::tr("Spade"), SuitSquareSpade},
    {FortuneStreetData::tr("Heart"), SuitSquareHeart},
    {FortuneStreetData::tr("Diamond"), SuitSquareDiamond},
    {FortuneStreetData::tr("Club"), SuitSquareClub},
    {FortuneStreetData::tr("Spade (Change-of-Suit)"), ChangeOfSuitSquareSpade},
    {FortuneStreetData::tr("Heart (Change-of-Suit)"), ChangeOfSuitSquareHeart},
    {FortuneStreetData::tr("Diamond (Change-of-Suit)"), ChangeOfSuitSquareDiamond},
    {FortuneStreetData::tr("Club (Change-of-Suit)"), ChangeOfSuitSquareClub},
    {FortuneStreetData::tr("Take-A-Break"), TakeABreakSquare},
    {FortuneStreetData::tr("Boon"), BoonSquare},
    {FortuneStreetData::tr("Boom"), BoomSquare},
    {FortuneStreetData::tr("Stockbroker"), StockBrokerSquare},
    {FortuneStreetData::tr("Roll On"), RollOnSquare},
    {FortuneStreetData::tr("Arcade"), ArcadeSquare},
    {FortuneStreetData::tr("Switch"), SwitchSquare},
    {FortuneStreetData::tr("Cannon"), CannonSquare},
    {FortuneStreetData::tr("Backstreet A"), BackStreetSquareA},
    {FortuneStreetData::tr("Backstreet B"), BackStreetSquareB},
    {FortuneStreetData::tr("Backstreet C"), BackStreetSquareC},
    {FortuneStreetData::tr("Backstreet D"), BackStreetSquareD},
    {FortuneStreetData::tr("Backstreet E"), BackStreetSquareE},
    {FortuneStreetData::tr("One-Way Alley Door A"), OneWayAlleyDoorA},
    {FortuneStreetData::tr("One-Way Alley Door B"), OneWayAlleyDoorB},
    {FortuneStreetData::tr("One-Way Alley Door C"), OneWayAlleyDoorC},
    {FortuneStreetData::tr("One-Way Alley Door D"), OneWayAlleyDoorD},
    {FortuneStreetData::tr("Lift/Magmalice Start"), LiftMagmaliceSquareStart},
    {FortuneStreetData::tr("Lift End"), LiftSquareEnd},
    {FortuneStreetData::tr("Magmalice"), MagmaliceSquare},
    {FortuneStreetData::tr("One-Way Alley End"), OneWayAlleySquare},
    {FortuneStreetData::tr("Event"), EventSquare},
    {FortuneStreetData::tr("Vacant Plot"), VacantPlot}
};
OrderedMap<QString, quint8> textToShopTypes = {
    {"",0},
    {FortuneStreetData::tr("Scrap-paper shop"),5},
    {FortuneStreetData::tr("Wool shop"),6},
    {FortuneStreetData::tr("Bottle store"),7},
    {FortuneStreetData::tr("Secondhand book shop"),8},
    {FortuneStreetData::tr("Scrap-metal supplier"),9},
    {FortuneStreetData::tr("Stationery shop"),10},
    {FortuneStreetData::tr("General store"),11},
    {FortuneStreetData::tr("Florist's"),12},
    {FortuneStreetData::tr("Ice-cream shop"),13},
    {FortuneStreetData::tr("Comic-book shop"),14},
    {FortuneStreetData::tr("Dairy"),15},
    {FortuneStreetData::tr("Doughnut shop"),16},
    {FortuneStreetData::tr("Pizza shack"),17},
    {FortuneStreetData::tr("Bakery"),18},
    {FortuneStreetData::tr("Grocery store"),19},
    {FortuneStreetData::tr("Pharmacy"),20},
    {FortuneStreetData::tr("Fish market"),21},
    {FortuneStreetData::tr("Toy shop"),22},
    {FortuneStreetData::tr("Bookshop"),23},
    {FortuneStreetData::tr("Cosmetics boutique"),24},
    {FortuneStreetData::tr("T-shirt shop"),25},
    {FortuneStreetData::tr("Fruit stall"),26},
    {FortuneStreetData::tr("Photography studio"),27},
    {FortuneStreetData::tr("Coffee shop"),28},
    {FortuneStreetData::tr("Butcher shop"),29},
    {FortuneStreetData::tr("Restaurant"),30},
    {FortuneStreetData::tr("Barbershop"),31},
    {FortuneStreetData::tr("Hat boutique"),32},
    {FortuneStreetData::tr("Hardware store"),33},
    {FortuneStreetData::tr("Gift shop"),34},
    {FortuneStreetData::tr("Launderette"),35},
    {FortuneStreetData::tr("Shoe shop"),36},
    {FortuneStreetData::tr("Clothing store"),37},
    {FortuneStreetData::tr("Optician's"),38},
    {FortuneStreetData::tr("Clockmaker's"),39},
    {FortuneStreetData::tr("Furniture shop"),40},
    {FortuneStreetData::tr("Sports shop"),41},
    {FortuneStreetData::tr("Locksmith's"),42},
    {FortuneStreetData::tr("Glassmaker's"),43},
    {FortuneStreetData::tr("Sushi restaurant"),44},
    {FortuneStreetData::tr("Art gallery"),45},
    {FortuneStreetData::tr("Leatherware boutique"),46},
    {FortuneStreetData::tr("Pet shop"),47},
    {FortuneStreetData::tr("Nail salon"),48},
    {FortuneStreetData::tr("Spice shop"),49},
    {FortuneStreetData::tr("Music shop"),50},
    {FortuneStreetData::tr("Surf shop"),51},
    {FortuneStreetData::tr("Boating shop"),52},
    {FortuneStreetData::tr("Cartographer's"),53},
    {FortuneStreetData::tr("Alloy rims shop"),54},
    {FortuneStreetData::tr("Fashion boutique"),55},
    {FortuneStreetData::tr("Waxworks"),56},
    {FortuneStreetData::tr("Lens shop"),57},
    {FortuneStreetData::tr("Kaleidoscope shop"),58},
    {FortuneStreetData::tr("Crystal ball shop"),59},
    {FortuneStreetData::tr("Gemstone supplier"),61},
    {FortuneStreetData::tr("Taxidermy studio"),62},
    {FortuneStreetData::tr("Antiques dealer's"),65},
    {FortuneStreetData::tr("Goldsmith's"),68},
    {FortuneStreetData::tr("Fossil shop"),70},
    {FortuneStreetData::tr("Music-box shop"),72},
    {FortuneStreetData::tr("Marionette workshop"),75},
    {FortuneStreetData::tr("Health shop"),76},
    {FortuneStreetData::tr("Organic food shop"),80},
    {FortuneStreetData::tr("Bridal boutique"),81},
    {FortuneStreetData::tr("Autograph shop"),85},
    {FortuneStreetData::tr("Meteorite shop"),90},
    {FortuneStreetData::tr("Department store"),98}
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

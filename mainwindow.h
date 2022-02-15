#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QLineEdit>
#include <QMainWindow>
#include <QUndoStack>
#include "fortuneavenuegraphicsscene.h"
#include "fortunestreetdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication& app);
    ~MainWindow();
private:
    Ui::MainWindow *ui;

    QVector<QLineEdit *> waypointStarts;
    QVector<QWidget *> waypointDests;

    FortuneAvenueGraphicsScene *scene;

    QUndoStack *undoStack;

    int zoomPercent = 100;
    BoardFile initialFile;
    int previouslyVisitedSquareId;
    const QString defaultPriceFunction = QString("x * ( -0.15 * 0.2^(x/200) + 0.2 )");
    QString priceFunction = defaultPriceFunction;

    int calcShopPriceFromValue(const QString &function, int value);
    void loadFile(const QString &fname);
    void loadFile(const BoardFile &file);
    BoardFile exportFile();
    void registerSquareSidebarEvents();
    void updateSquareSidebar();
    template<typename Func> void updateSquare(Func func);
    void updateWaypoints();
    void newFile();
    void openFile();
    /**
     * @return whether file was saved
     */
    bool saveFile();
    /**
     * @return whether file was saved
     */
    bool saveFileAs();
    void calcStockPrices();
    void verifyBoard();
    void autoPath();
    void screenshot();
    void addSquare();
    void removeSquare();
    int calcSnapSizeFromInput();
    void updateSnapSize();
    void updateZoom();
    void updateDestinationUI();
    void followWaypoint(int destinationId);
    void selectNext();
    void selectPrevious();
    void selectAll();
    void clearWaypoint(SquareItem *item, int waypointId);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QLineEdit>
#include <QMainWindow>
#include "fortuneavenuegraphicsscene.h"
#include "fortunestreetdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;

    QVector<QLineEdit *> waypointStarts;
    QVector<QWidget *> waypointDests;

    FortuneAvenueGraphicsScene *scene;

    int zoomPercent = 100;

    void loadFile(const BoardFile &file);
    BoardFile exportFile();
    void registerSquareSidebarEvents();
    void updateSquareSidebar();
    template<typename Func> void updateSquare(Func func);
    void updateWaypoints();
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
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
};
#endif // MAINWINDOW_H

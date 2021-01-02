#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QMainWindow>
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

    QGraphicsScene *scene;

    void loadFile(BoardFile file);
    BoardFile exportFile();
private slots:
    void updateSquareSidebar();
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
};
#endif // MAINWINDOW_H

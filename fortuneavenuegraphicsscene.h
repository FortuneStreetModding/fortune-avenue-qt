#ifndef FORTUNEAVENUEGRAPHICSSCENE_H
#define FORTUNEAVENUEGRAPHICSSCENE_H

#include <QGraphicsScene>

class FortuneAvenueGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    FortuneAvenueGraphicsScene(qreal x, qreal y, qreal w, qreal h, QObject *parent = nullptr);
    FortuneAvenueGraphicsScene(const QRectF &rect, QObject *parent = nullptr);
    FortuneAvenueGraphicsScene(QObject *parent = nullptr);
    ~FortuneAvenueGraphicsScene() override;
    int getSnapSize() const;
    void setSnapSize(int value);
private:
    int snapSize = 1;
};

#endif // FORTUNEAVENUEGRAPHICSSCENE_H

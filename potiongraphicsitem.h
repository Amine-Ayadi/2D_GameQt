#ifndef POTIONGRAPHICSITEM_H
#define POTIONGRAPHICSITEM_H

#include <QGraphicsItem>

class PotionGraphicsItem : public QGraphicsItem
{
public:
    PotionGraphicsItem();
    PotionGraphicsItem(int xPos, int yPos);
    ~PotionGraphicsItem();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int _gridSize;
    int _xPos;
    int _yPos;
    int _realPositionX;
    int _realPositionY;
    int _frameWidth;
    int _frameHeight;
    int _currentFrame;
    QPixmap _potionPixmap;
};
#endif // POTIONGRAPHICSITEM_H

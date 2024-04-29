#ifndef ENEMYGRAPHICSITEM_H
#define ENEMYGRAPHICSITEM_H

#include <QGraphicsItem>

class EnemyGraphicsItem : public QGraphicsItem
{
public:

    EnemyGraphicsItem();
    EnemyGraphicsItem(int xPos, int yPos, float health);
    ~EnemyGraphicsItem();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    int getXPos() { return _xPos;}
    void setXPos(int yPos) { _xPos = yPos;}
    int getYPos() { return _yPos;}
    void setYPos(int yPos) { _yPos = yPos;}

    private:
        int _gridSize;
        int _xPos;
        int _yPos;
        int _realPositionX;
        int _realPositionY;
        int _frameWidth;
        int _frameHeight;
        int _currentFrame;
        int _health;
        QPixmap _enemyPixmap;

    };

#endif // ENEMYGRAPHICSITEM_H

#ifndef PROTAGONISTGRAPHICS_H
#define PROTAGONISTGRAPHICS_H

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>
#include <QMovie>
#include <QObject>

class AnimationManager;

class ProtagonistGraphics : public QGraphicsItem
{

public:
    ProtagonistGraphics();
    ~ProtagonistGraphics();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public:
    void setMovie(QMovie *newMovie);

private:
    void animate();

    QTimer*_timer;
    QPixmap _characterPixmap;
    QMovie* _movie;
    int _gridSize;
    int _frameWidth;
    int _frameHeight;
    int _currentFrame;

};


#endif // PROTAGONISTGRAPHICS_H



#include "protagonistgraphics.h"
#include "Config.h"
#include "worldInstance.h"
#include "animationmanager.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWheelEvent>
#include <QDebug>
#include <QTimer>


auto w_instance = WorldInstance::getInstance();

ProtagonistGraphics::ProtagonistGraphics()
{
    _gridSize = Config::TILE_SIZE;
    _frameWidth = 8;
    _frameHeight = 8;
}

ProtagonistGraphics::~ProtagonistGraphics()
{
    qDebug() << "Protagonist Destroyed";
}

QRectF ProtagonistGraphics::boundingRect() const
{
    return QRectF(w_instance->getProtagonist()->getXPos() * _gridSize,
                  w_instance->getProtagonist()->getYPos() * _gridSize,
                  _gridSize,
                  _gridSize);
}

void ProtagonistGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    // Draw Protagonist
    QRectF targetRect = boundingRect();
    QRect targetRectInt = targetRect.toRect();
    if(w_instance->getAtomicKeyAction()){
        painter->drawPixmap(targetRectInt,
                            w_instance->animationManager()->getPixmapFromFrame(w_instance->animationManager()->getCurrentFrame()));
    }
    painter->drawPixmap(targetRectInt,
                        w_instance->animationManager()->getPixmapFromFrame(w_instance->animationManager()->getCurrentFrame()));

    // Draw the life bar
    qreal lifeBarWidth = _gridSize;
    qreal lifeBarHeight = 5;  // Adjust the height of the life bar
    qreal lifePercentage = static_cast<qreal>(w_instance->getProtagonist()->getHealth()) / 100.0;

    QRectF lifeBarRect = QRectF(w_instance->getProtagonist()->getXPos() * _gridSize,
                                w_instance->getProtagonist()->getYPos() * _gridSize - lifeBarHeight - 2,
                                lifeBarWidth * lifePercentage,
                                lifeBarHeight);
    painter->setPen(Qt::black);
    painter->drawRect(lifeBarRect);
    painter->fillRect(lifeBarRect, Qt::green);
}


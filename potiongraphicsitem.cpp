#include "potiongraphicsitem.h"
#include "worldInstance.h"
#include "Config.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QRectF>
#include <QApplication>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>


PotionGraphicsItem::PotionGraphicsItem()
{

}

PotionGraphicsItem::PotionGraphicsItem(int xPos, int yPos)
{
    _potionPixmap = QPixmap(":/images/health.png");
    _gridSize = Config::TILE_SIZE;
    _realPositionX = xPos;
    _realPositionY = yPos;
    _xPos = xPos *_gridSize;
    _yPos = yPos * _gridSize;
    _frameWidth = 8;
    _frameHeight = 8;
    _currentFrame = 0;
}

PotionGraphicsItem::~PotionGraphicsItem()
{
    qDebug() << "Potions Destroyed";
}

QRectF PotionGraphicsItem::boundingRect() const
{
    return QRectF(_xPos,
                  _yPos,
                  _gridSize,
                  _gridSize);
}

void PotionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    auto w_instance = WorldInstance::getInstance();
    int char_x = w_instance->getProtagonist()->getXPos();
    int char_y = w_instance->getProtagonist()->getYPos();
    int colStart = char_x - Config::RENDER_START_OFFSET;
    int colEnd = char_x + Config::RENDER_END_OFFSET;
    int rowStart = char_y - Config::RENDER_START_OFFSET;
    int rowEnd = char_y + Config::RENDER_END_OFFSET;

    if(_realPositionX < colEnd
        && _realPositionX > colStart
        && _realPositionY > rowStart
        && _realPositionY < rowEnd) {

        QRectF pixelRect = QRectF(_xPos,
                                  _yPos,
                                  _gridSize,
                                  _gridSize);

        QRectF sourceRect(_currentFrame * _frameWidth, 0, _frameWidth, _frameHeight);
        painter->drawPixmap(pixelRect, _potionPixmap, sourceRect);
    }
}

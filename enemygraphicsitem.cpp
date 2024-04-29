#include "enemygraphicsitem.h"
#include "Config.h"
#include "worldInstance.h"
#include <QGraphicsItem>
#include <QPainter>
#include <QRectF>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>

EnemyGraphicsItem::EnemyGraphicsItem()
{

}

EnemyGraphicsItem::EnemyGraphicsItem(int xPos, int yPos, float health)
{
    _gridSize = Config::TILE_SIZE;
    _realPositionX = xPos;
    _realPositionY = yPos;
    _xPos = xPos *_gridSize;
    _yPos = yPos * _gridSize;
    _enemyPixmap = QPixmap(":/images/enem.png");
    _health = health;
    _frameWidth = 8;   // Adjust according to your sprite sheet
    _frameHeight = 8;

    // Initialize animation variables
    _currentFrame = 0;
}

EnemyGraphicsItem::~EnemyGraphicsItem()
{
    qDebug() << "Enemies Destroyed";
}

QRectF EnemyGraphicsItem::boundingRect() const
{
    return QRectF(_xPos,
                  _yPos,
                  _gridSize,
                  _gridSize);
}

void EnemyGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto w_instance = WorldInstance::getInstance();

    int char_x = w_instance->getProtagonist()->getXPos();
    int char_y = w_instance->getProtagonist()->getYPos();
    int colStart = char_x - Config::RENDER_START_OFFSET;
    int colEnd = char_x + Config::RENDER_END_OFFSET;
    int rowStart = char_y - Config::RENDER_START_OFFSET;
    int rowEnd = char_y + Config::RENDER_END_OFFSET;

    if(_realPositionX >= colEnd
        || _realPositionX <= colStart
        || _realPositionY <= rowStart
        || _realPositionY >= rowEnd)
    {
        return;

    }

    QRectF pixelRect = QRectF(_xPos,
                              _yPos,
                              _gridSize,
                              _gridSize);

    QRectF sourceRect(_currentFrame * _frameWidth, 0, _frameWidth, _frameHeight);
    painter->drawPixmap(pixelRect, _enemyPixmap, sourceRect);

    // Draw the life bar
    qreal lifeBarWidth = _gridSize;
    qreal lifeBarHeight = 5;  // Adjust the height of the life bar
    qreal lifePercentage = static_cast<qreal>(_health) / 100.0;

    QRectF lifeBarRectFull = QRectF(_xPos,
                                _yPos - lifeBarHeight - 2,
                                lifeBarWidth,
                                lifeBarHeight);
    QRectF lifeBarRect = QRectF(_xPos,
                                _yPos - lifeBarHeight - 2,
                                lifeBarWidth * lifePercentage,
                                lifeBarHeight);
    painter->setPen(Qt::black);
    painter->drawRect(lifeBarRectFull);
    painter->drawRect(lifeBarRect);
    painter->fillRect(lifeBarRect, Qt::green);

}

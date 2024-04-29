#include "mapgraphicsitem.h"
#include "Config.h"
#include "qgraphicssceneevent.h"
#include "worldInstance.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QRectF>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsBlurEffect>



MapGraphicsItem::MapGraphicsItem(int rows, int cols, const std::vector<std::unique_ptr<Tile>>& tiles)
    : _tiles(tiles)
{
    qDebug() << "MapGraphicsItem constructor" ;
    _gridSize = Config::TILE_SIZE;
    _rows = rows;
    _columns = cols;
    _road = QPixmap(":/images/GrassTile.png");
    _wall = QPixmap(":/images/WaterTile.png");
    _bush = QPixmap(":/images/bush.png");
    _rocks = QPixmap(":/images/rocks.png");
    setBrush(QColor(255, 255, 112, 50));
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    qDebug() << "Map: number of tiles = " << _tiles.size();
}

QRectF MapGraphicsItem::boundingRect() const {
    return QRectF(0, 0, _columns * _gridSize, _rows * _gridSize);
}

void MapGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    auto w_instance = WorldInstance::getInstance();

    // param for selected area around the player
    int char_x = w_instance->getProtagonist()->getXPos();
    int char_y = w_instance->getProtagonist()->getYPos();
    int colStart = char_x - Config::RENDER_START_OFFSET;
    int colEnd = char_x + Config::RENDER_END_OFFSET;
    int rowStart = char_y - Config::RENDER_START_OFFSET;
    int rowEnd = char_y + Config::RENDER_END_OFFSET;

    for (int i = colStart; i < colEnd; ++i)
    {
        for (int j = rowStart; j < rowEnd; ++j)
        {
            int index = j * _columns + i;
            if (i < _rows
                && j < _columns
                && i >= 0
                && j >= 0)
            {
                if (_tiles[index])
                {
                    QRectF _pixelRect = QRectF(_tiles[index]->getXPos() * _gridSize, _tiles[index]->getYPos() * _gridSize, _gridSize, _gridSize);
                    if(w_instance->getAtomicPathAction()){
                        for (auto& node: w_instance->getNodes()){
                            if (_tiles[index]->getXPos() == node.getX() && _tiles[index]->getYPos() == node.getY()){
                                // qDebug() << "to highlight pathfinder ";
                                // _road.fill(QColor(0, 0, 0, 128));
                                // _bush.fill(QColor(0, 0, 0, 128));
                                // _wall.fill(QColor(0, 0, 0, 128));
                                painter->setOpacity(0.5);
                            }
                            else {
                                // _road.fill(QColor(0, 0, 0, 0));
                                // _bush.fill(QColor(0, 0, 0, 0));
                                // _wall.fill(QColor(0, 0, 0, 0));
                                painter->setOpacity(1.0);
                            }
                        }
                    }
                    // WALLS
                    if (_tiles[index]->getValue() == INFINITY || _tiles[index]->getValue() >=  Config::WALL_LIMIT)
                    {
                        painter->drawPixmap(_tiles[index]->getXPos() * _gridSize,
                                            _tiles[index]->getYPos() * _gridSize,
                                            _gridSize,
                                            _gridSize,
                                            _wall);
                    }
                    // BUSH
                    else if (_tiles[index]->getValue() < 0.3f && _tiles[index]->getValue() > 0.26f)
                    {
                        painter->drawPixmap(_tiles[index]->getXPos() * _gridSize,
                                            _tiles[index]->getYPos() * _gridSize,
                                            _gridSize,
                                            _gridSize,
                                            _road);
                        painter->drawPixmap(_tiles[index]->getXPos() * _gridSize,
                                            _tiles[index]->getYPos() * _gridSize,
                                            _gridSize,
                                            _gridSize,
                                            _bush);

                    }
                    // ROAD
                    else
                    {
                        painter->drawPixmap(_tiles[index]->getXPos() * _gridSize,
                                            _tiles[index]->getYPos() * _gridSize,
                                            _gridSize,
                                            _gridSize,
                                            _road);
                    }
                    if (w_instance->getAtomicPathReady()){
                        qDebug() << "Drawing map: mouse clicked for pathfinder";
                        for (auto& node : w_instance->getNodes()){
                            qDebug() << "pathfinder node" << node.getX() << ", " << node.getY();
                            painter->setPen(QPen(QColor(255, 0, 0), 2));
                            painter->drawRect(_pixelRect);
                            painter->fillRect(_pixelRect, QColor(255, 0, 0));
                            painter->setBrush(QColor(_tiles[index]->getValue() * 255.0f, _tiles[index]->getValue() * 255.0f, _tiles[index]->getValue() * 255.0f));
                        }
                        if (_tiles[index]->getXPos() == _selectedCol
                            && _tiles[index]->getYPos() == _selectedRow)
                        {
                            qDebug() << "Drawing map: mouse selected";
                            painter->setPen(QPen(QColor(255, 0, 0), 2));
                            painter->drawRect(_pixelRect);
                            painter->fillRect(_pixelRect, QColor(255, 0, 0));
                            painter->setBrush(QColor(_tiles[index]->getValue() * 255.0f, _tiles[index]->getValue() * 255.0f, _tiles[index]->getValue() * 255.0f));
                        }
                    }
                }
                // OUT OF BOUND
                else
                {
                    painter->drawPixmap(i * _gridSize,
                                        j * _gridSize,
                                        _gridSize,
                                        _gridSize,
                                        _rocks);
                }
            }
        }
    }
    update();
}

void MapGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    auto w_instance = WorldInstance::getInstance();
    _selectedRow = static_cast<int>(event->scenePos().y() / Config::TILE_SIZE);
    _selectedCol = static_cast<int>(event->scenePos().x() / Config::TILE_SIZE);
    w_instance->goToDestination(_selectedCol, _selectedRow);
    setCursor(Qt::ClosedHandCursor);
    return;
}



void MapGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->scenePos(), event->buttonDownScreenPos(Qt::LeftButton))
            .length() < QApplication::startDragDistance()) {
        return;
    }
}

void MapGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "Mouse Released" ;
    setCursor(Qt::OpenHandCursor);
    _tileSelected = false;
}

void MapGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    int row = static_cast<int>(event->scenePos().y() / Config::TILE_SIZE);
    int col = static_cast<int>(event->scenePos().x() / Config::TILE_SIZE);
    qDebug() << "Map Hovered";
    update();
}

void MapGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    setGraphicsEffect(nullptr);
}





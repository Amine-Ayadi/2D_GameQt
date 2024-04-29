#ifndef MAPGRAPHICSITEM_H
#define MAPGRAPHICSITEM_H

#include "world.h"
#include "node.h"
#include <QPainter>
#include <QGraphicsItem>

class MapGraphicsItem : public QGraphicsItem {

public:
    MapGraphicsItem(int rows, int cols, const std::vector<std::unique_ptr<Tile>>& tiles);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QBrush getBrush() const { return _brush;}
    void setBrush(const QBrush &newBrush) {_brush = newBrush;}

private:
    int _gridSize;
    int _rows;
    int _columns;
    int _selectedRow;
    int _selectedCol;
    bool _tileSelected;
    QBrush _brush;
    std::vector<Node> _nodes;
    const std::vector<std::unique_ptr<Tile>>& _tiles;

    QPixmap _road;
    QPixmap _wall;
    QPixmap _bush;
    QPixmap _rocks;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};

#endif // MAPGRAPHICSITEM_H

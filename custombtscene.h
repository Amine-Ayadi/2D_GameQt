#ifndef CUSTOMBTSCENE_H
#define CUSTOMBTSCENE_H

#include <QPainter>
#include <QGraphicsItem>

class QPainter;
class QString;

class CustomBtScene : public QGraphicsItem
{
public:
    CustomBtScene(int width,
                  int height,
                  const QString& map = QString(),
                  const QString& text = "",
                  QGraphicsItem* parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    QRectF boundingRect() const override;
    QBrush getBrush() const { return _brush;}
    void setBrush(const QBrush &newBrush) {_brush = newBrush;}

    QString map() const;

    void setMap(const QString &newMap);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    qreal _cornerRadius;
    QString _text;
    QBrush _brush;
    QString _map;
    QFont _font;
    bool isMap;
    int _width;
    int _height;

};

#endif // CUSTOMBTSCENE_H

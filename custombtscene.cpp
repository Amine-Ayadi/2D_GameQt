#include "custombtscene.h"
#include "worldInstance.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QFontDatabase>

CustomBtScene::CustomBtScene(int width,
                             int height,
                             const QString& map,
                             const QString &text, QGraphicsItem *parent)
{
    Q_UNUSED(text)
    Q_UNUSED(parent)
    _cornerRadius = 10;
    _text = text;
    if (map.isNull()) {
        isMap = false;
        // qDebug() << "Received a null QPixmap.";
    } else {
        isMap = true;
        setMap(map);
        // qDebug() << "Received a valid QPixmap." << map;
    }
    _width = width;
    _height = height;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setBrush(QColor(255, 133, 112, 50));
    QString fontPath = ":/other/AGoblinAppears-o2aV.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);

    // Check if the font was loaded successfully
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            _font = QFont(fontFamily, 18);
            // qDebug() << "Font loaded successfully!";
        }
    } else {
        qDebug() << "Failed to load the font!";
        _font = QFont("Bitter", 18); ;
    }

}

QRectF CustomBtScene::boundingRect() const {
    return QRectF(0, 0, _width, _height);
}

void CustomBtScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    const int studSize = 3;
    Q_UNUSED(widget)
    Q_UNUSED(studSize)
    Q_UNUSED(option)
    QRectF rect = boundingRect();
    QString buttonText = _text;
    // QFont font("Bitter", 18);
    QRectF textRect = rect.adjusted(5, 5, -5, -5);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 160, 122), 4));

    if(!isMap){
        painter->setBrush(getBrush());
        // painter->setPen(QPen(QColor(255, 160, 122), 3));
        painter->drawRoundedRect(boundingRect(), _cornerRadius, _cornerRadius);
        // Draw studded border
        painter->setFont(_font);
        painter->drawText(textRect, Qt::AlignCenter, buttonText);
    } else {
        // qDebug() << "crashes here" << _map;
        painter->setPen(QPen(QColor(0, 0, 0), 1));
        painter->drawPixmap(0, 0, _width, _height, _map);
        painter->drawRect(0,0,_width,_height);
    }
}

void CustomBtScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // QGraphicsItem::mousePressEvent(event);
    auto w_instance = WorldInstance::getInstance();
    if(event->button() == Qt::LeftButton) {
        // qDebug() << "ClickableItem clicke\d!";
        if(_text == "START"){
            qDebug() << "START pressed: ";
            emit(w_instance->startPressed());
        }
        else if(_text == "EXIT"){
            qDebug() << "EXIT pressed: ";
            emit(w_instance->exitPressed());
        }
        if(isMap){
            qDebug() << "MAP pressed: " << _map;
            w_instance->setMap(_map);
            emit(w_instance->mapSelected(_map));
        }
    }
    setBrush(QColor(255, 160, 112, 70));
    update();
}

void CustomBtScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
    }
    setBrush(QColor(255, 133, 112, 50));
    update();
}

void CustomBtScene::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event)
    if(isMap){
        QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
        blur->setBlurRadius(4);
        setGraphicsEffect(blur);
    } else
        setBrush(QColor(255, 170, 112, 20));
    update();
}

void CustomBtScene::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event)
    if(isMap){
        setGraphicsEffect(nullptr);
    } else
        setBrush(QColor(255, 133, 112, 50));
    update();
}

void CustomBtScene::setMap(const QString &newMap)
{
    _map = newMap;
}

QString CustomBtScene::map() const
{
    return _map;
}



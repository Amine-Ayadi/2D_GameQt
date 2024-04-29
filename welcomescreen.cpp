#include "welcomescreen.h"
#include "worldInstance.h"
#include "custombtscene.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QWheelEvent>
#include <QSlider>
#include <QPainter>

WelcomeScreen::WelcomeScreen(const QString &name, QWidget *parent)
   : GameScreen(name, parent)
{
    qDebug() << "WelcomeScreen constructor";
    setFrameStyle(Sunken | StyledPanel);
    _view = new QGraphicsView(this);
    _view->setDragMode(QGraphicsView::ScrollHandDrag);
    _view->setStyleSheet("background: transparent");
    _view->setAlignment(Qt::AlignCenter);
    _view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    _view->setRenderHint(QPainter::Antialiasing, true);
    _view->setRenderHint(QPainter::LosslessImageRendering);
    _view->setRenderHint(QPainter::TextAntialiasing);
    _view->setDragMode(QGraphicsView::RubberBandDrag);
    _view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    _view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    _view->setTransformationAnchor(QGraphicsView::NoAnchor);

    _maps[0] = QString(":/images/maze1.png");
    _maps[1] = QString(":/images/maze2.png");
    _maps[2] = QString(":/images/maze3.png");
    _maps[3] = QString(":/images/worldmap.png");
    _maps[4] = QString(":/images/worldmap4.png");
    _maps[5] = QString(":/images/image8x8.png");

    auto w_instance = WorldInstance::getInstance();
    _view->setScene(new QGraphicsScene(this));
    _scene = _view->scene();
    populateScene();

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(_view, 1, 0);
    setLayout(topLayout);

    qDebug() << "WelcomeScreen constructor";
}

WelcomeScreen::~WelcomeScreen()
{
    delete _scene;
    delete _view;
    delete _startBt;
    delete _exitBt;
    qDebug() << "WelcomeScreen Destroyed";
}

void WelcomeScreen::populateScene()
{
    QPixmap backgroundPixmap(":/images/bg_image.jpg");
    auto w_instance = WorldInstance::getInstance();
    QGraphicsPixmapItem* backgroundItem = new QGraphicsPixmapItem(backgroundPixmap);
    backgroundItem->setPos(_scene->width()/2, _scene->height()/2);

    _scene->addItem(backgroundItem);
    _startBt = new CustomBtScene(200,80,QString(),"START");
    _startBt->setPos(_scene->width()/2 - _startBt->boundingRect().width()/2,
                     _scene->height()*0.60 - _startBt->boundingRect().height()/2);
    _exitBt = new CustomBtScene(200,80,QString(),"EXIT");
    _exitBt->setPos(_scene->width()/2 - _startBt->boundingRect().width()/2,
                    _scene->height()*0.75 - _startBt->boundingRect().height()/2);

    _scene->addItem(_startBt);
    _scene->addItem(_exitBt);

    // Map menu to choose from:
    int pictureWidth = 50;
    int pictureHeight = 50;
    for(int i = 0; i < 6; i++){
        QString pixmap = getMap(i);
        QPixmap pic = QPixmap(pixmap);
        QPixmap scaledPixmap = pic.scaled(pictureWidth, pictureHeight, Qt::KeepAspectRatio);
        CustomBtScene* map = new CustomBtScene(pictureWidth,pictureHeight,pixmap,"");
        map->setPos(- i % 6 * pictureWidth + _scene->width()/2 + _startBt->boundingRect().width()/2,
                    _scene->height()*0.83);
        _scene->addItem(map);
    }

}

const QString WelcomeScreen::getMap(int index) const
{
    QMap<int, QString>::const_iterator it = _maps.constBegin();

    while (it != _maps.constEnd() && it.key() != index)
    {
        ++it;
    }

    if (it != _maps.constEnd())
    {
        // qDebug() << "Index: " << it.key() << ", Path: " << it.value();
        return it.value();
    }
    else
    {
        // qDebug() << "Index not found: " << index;
        return QString();
    }
}

QGraphicsView *WelcomeScreen::view() const
{
    return static_cast<QGraphicsView *>(_view);
}



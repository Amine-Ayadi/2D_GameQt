#include "gamescreen.h"

#include <QGraphicsView>
#include <QGraphicsScene>

GameScreen::GameScreen(const QString &name, QWidget *parent)
    : QFrame(parent), _view(new QGraphicsView(this))
{
    qDebug() << "GameScreen constructor";
}

GameScreen::~GameScreen()
{
    delete _view;
}

QGraphicsView *GameScreen::view() const
{
    return _view;
}

#include <QThread>
#include "gamecontroller.h"

GameController::GameController(QObject *parent)
    : QObject{parent}
{
    qDebug () << "GameController constructor";
}
GameController::~GameController()
{
}

void GameController::handleShowOptions()
{
    emit showOptionsRequested();
    emit addTextConsole("Options clicked");
}

void GameController::handleSelectedTile()
{
    qDebug() << "GameController: handleSelectedTile()";
}









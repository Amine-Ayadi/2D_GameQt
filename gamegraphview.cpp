#include "gamegraphview.h"
#include "mapgraphicsitem.h"
#include "worldInstance.h"
#include "protagonistgraphics.h"
#include "potiongraphicsitem.h"
#include "enemygraphicsitem.h"
#include "Config.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <memory>
#include <QFile>
#include <QThread>
#include <QWheelEvent>
#include <QSlider>

using namespace std;

GameGraphView::GameGraphView(const QString &name, QWidget *parent)
    : GameScreen(name, parent)
{
    qDebug() << "GameGraphView constructor";
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
    _view->setScene(new QGraphicsScene(this));
    _scene = _view->scene();

    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(_view, 1, 0);
    setLayout(topLayout);

    auto w_instance = WorldInstance::getInstance();
    connect(w_instance.get(), &WorldInstance::mapSelected, this, &GameGraphView::handleSelectedMap);
    connect(w_instance.get(), &WorldInstance::positionUpdated, this, &GameGraphView::handlePositionUpdated);
    connect(w_instance.get(), &WorldInstance::positionAIUpdated, this, &GameGraphView::handlePositionPathfinder);
    connect(w_instance.get(), &WorldInstance::animate, this, &GameGraphView::handleAnimate);

    // zoom mechanism
    _zoomSlider = new QSlider;
    _zoomSlider->setMinimum(0);
    _zoomSlider->setMaximum(500);
    _zoomSlider->setValue(250);
    _zoomSlider->setTickPosition(QSlider::TicksRight);
    connect(_zoomSlider, &QAbstractSlider::valueChanged, this, &GameGraphView::setupMatrix);
    setupMatrix();
}

GameGraphView::~GameGraphView()
{

}

void GameGraphView::populateScene()
{
    auto w_instance = WorldInstance::getInstance();
    if (!w_instance)
    {
        qDebug() << "WorldInstance empty: Could not populate!!!";
        return;
    }

    // GAME MAP:
    _mapGraphics = std::make_unique<MapGraphicsItem>(w_instance->getRows(),
                                                     w_instance->getCols(),
                                                     w_instance->getTiles());
    _mapGraphics->setZValue(1);
    _scene->addItem(_mapGraphics.get());

    // PROTAGONIST
    _protagonistGraphics = std::make_unique<ProtagonistGraphics>();
    WorldInstance::getInstance()->pushActionToStack(WorldInstance::ActionType::IDLE);
    _protagonistGraphics->setZValue(2);
    _protagonistGraphics->setFlag(QGraphicsItem::ItemIsFocusable);
    _protagonistGraphics->setFocus();
    _scene->addItem(_protagonistGraphics.get());
    // POTIONS
    for (auto& potionPtr: w_instance->getHealthPotions()) {
        _potionsGraphics.emplace_back(std::make_unique<PotionGraphicsItem>(potionPtr->getXPos(),potionPtr->getYPos()));

    }
    for(auto& potion: _potionsGraphics) {
        potion->setZValue(1);
        _scene->addItem(potion.get());
    }
    for (auto& enemPtr: w_instance->getEnemies()) {
        _enemiesGraphics.emplace_back(std::make_unique<EnemyGraphicsItem>(enemPtr->getXPos(),
                                                                          enemPtr->getYPos(),
                                                                          enemPtr->getValue()));
    }
    for(const auto& enemy: _enemiesGraphics) {
        enemy->setZValue(1);
        _scene->addItem(enemy.get());
    }

    w_instance->getGraphicsManager()->addEntity(std::move(_mapGraphics));
    w_instance->getGraphicsManager()->addEntity(std::move(_protagonistGraphics));
    qDebug() << "GameGraphView: components added";
}

void GameGraphView::handlePositionUpdated()
{
    setupMatrix();
    // qDebug() << "SIG:GameGraphView: Protagonist moving";
    _view->update();
}

void GameGraphView::handlePositionPathfinder()
{
    setupMatrix();
    // qDebug() << "SIG:GameGraphView: Pathfinder moving";
    _view->update();
}

void GameGraphView::handleSelectedMap(const QString &map)
{
    qDebug() << "SIG:GameGraphView: handle selected map" << map;
}

void GameGraphView::handleAnimate()
{
    // qDebug() << "TODO:: SIG:GameGraphView: Protagonist animated";
    _view->update();
}

void GameGraphView::handleZoomIn()
{
    _zoomSlider->setValue(_zoomSlider->value() + 1);
}

void GameGraphView::handleZoomOut()
{
    _zoomSlider->setValue(_zoomSlider->value() - 1);
}

void GameGraphView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
    {
        _zoomSlider->setValue(_zoomSlider->value() + 1);
    }
    else
    {
        _zoomSlider->setValue(_zoomSlider->value() - 1);
    }

    event->accept();
}

void GameGraphView::setupMatrix()
{
    auto w_instance = WorldInstance::getInstance();
    qreal scale = qPow(qreal(2), (_zoomSlider->value() - 250) / qreal(50));
    qreal _gridSize = w_instance->getGridSize();
    int char_x = w_instance->getProtagonist()->getXPos();
    int char_y = w_instance->getProtagonist()->getYPos();

    // area around protagonist to be rendered
    qreal startScene_x = (char_x - Config::RENDER_START_OFFSET) * _gridSize;
    qreal startScene_y = (char_y - Config::RENDER_START_OFFSET) * _gridSize;
    qreal endScene_x = (char_x + Config::RENDER_START_OFFSET) * _gridSize;
    qreal endScene_y = (char_y + Config::RENDER_START_OFFSET) * _gridSize;
    qreal SceneWidth = endScene_x - startScene_x;
    qreal SceneHeight = endScene_y - startScene_y;

    // center of rendered and whole view
    QPointF sceneCenter = QPointF(SceneWidth / 2, SceneHeight / 2);
    QPointF viewCenter = _view->mapToScene(_view->viewport()->rect().center());
    QPointF protCoords(char_x * _gridSize, char_y * _gridSize);
    _view->setSceneRect(QRectF(startScene_x, startScene_y, SceneWidth+_gridSize, SceneHeight+_gridSize));
    _scene->setSceneRect(QRectF(startScene_x, startScene_y, SceneWidth, SceneHeight));

    QTransform matrix;
    matrix.scale(scale, scale);
    matrix.translate(viewCenter.x() - sceneCenter.x(), viewCenter.y() - sceneCenter.y());
    _view->setTransform(matrix);
}




QGraphicsView *GameGraphView::view() const
{
    return static_cast<QGraphicsView *>(_view);
}


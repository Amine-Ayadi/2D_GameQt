#ifndef GAMEGRAPHVIEW_H
#define GAMEGRAPHVIEW_H

#include "world.h"
#include <memory>
#include <QFrame>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

#include "gamescreen.h"

class QGraphicsScene;
class TileView;
class WorldInstance;
class ProtagonistGraphics;
class QGraphicsView;
class QSlider;
class QLabel;
class EntityManager;
class ProtagonistGraphics;
class EnemyGraphicsItem;
class MapGraphicsItem;
class PotionGraphicsItem;


class GameGraphView : public GameScreen
{
    Q_OBJECT

public:
    explicit GameGraphView(const QString &name, QWidget *parent = nullptr);
    ~GameGraphView() override;
    QGraphicsView *view() const override;
    void populateScene() override;
    void setScene(QGraphicsScene *newScene) override {this->_scene = newScene;}
    QGraphicsScene *getScene() const override { return _scene; }

    void renderSelectedArea(QRectF tileRect);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    QGraphicsView* _view;
    QGraphicsScene* _scene;
    qreal _sceneWidth;
    qreal _sceneHeight;
    QSlider *_zoomSlider;

    std::shared_ptr<World> _world;
    std::vector<std::unique_ptr<Tile>> _tiles;
    std::vector<std::unique_ptr<PotionGraphicsItem>> _potionsGraphics;
    std::vector<std::unique_ptr<EnemyGraphicsItem>> _enemiesGraphics;
    std::unique_ptr<ProtagonistGraphics> _protagonistGraphics;
    std::unique_ptr<MapGraphicsItem> _mapGraphics;


public slots:
    void handlePositionUpdated();
    void handlePositionPathfinder();
    void handleAnimate();
    void handleZoomIn();
    void handleZoomOut();
    void setupMatrix();
    void handleSelectedMap(const QString& map);

};

// class GraphicsView : public QGraphicsView
// {
//     Q_OBJECT
// public:
//     GraphicsView(GameGraphView *v) : QGraphicsView(), view(v) { }


// private:
//     GameGraphView *view;
// };

#endif // GAMEGRAPHVIEW_H

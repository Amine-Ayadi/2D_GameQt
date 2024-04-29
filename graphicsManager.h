#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include "world.h"
#include <QGraphicsItem>

class MapGraphicsItem;

class GraphicsManager
{
public:
    GraphicsManager();
    ~GraphicsManager();

    void init();
    void update();
    void destroy();
    void removeGraphics(Tile* itemToRemove);
    void setScene(QGraphicsScene *newScene) {this->_scene = newScene;}
    QGraphicsScene *getScene() const { return _scene; }

    /***************************/
    template<typename T>
    void addEntity(std::unique_ptr<T> entity) {
        _entities.emplace_back(std::move(entity));
        isItemInRenderedArea[entity.get()].store(false);
    }
    template<typename T>
    void destroyEntity(T* entity) {
        _entities.erase(std::remove_if(_entities.begin(), _entities.end(), [entity](const std::unique_ptr<QGraphicsItem>& ptr){
                           return ptr.get() == entity;
                       }), _entities.end());
        // _scene->removeItem(entity);
        isItemInRenderedArea.erase(entity);
    }
    const std::vector<std::unique_ptr<QGraphicsItem>>& getEntities() const {
        return _entities;
    }
    void updateRenderedEntities();
    std::atomic_bool getIsItemInRenderedArea(QGraphicsItem* item, QGraphicsItem* mapItem);

private:
    QGraphicsScene* _scene;
    MapGraphicsItem* _mapGraphicsItem;
    std::map<Tile*, QGraphicsItem*> _graphicsitems;
    /***************************/
    std::map<QGraphicsItem*, std::atomic<bool>> isItemInRenderedArea;
    std::vector<std::unique_ptr<QGraphicsItem>> _entities;
    int _gridSize;
    int _rows;
    int _columns;
};

#endif // GRAPHICSMANAGER_H

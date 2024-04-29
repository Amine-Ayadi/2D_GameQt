#include "graphicsManager.h"
#include "worldInstance.h"
#include "mapgraphicsitem.h"
#include "protagonistgraphics.h"
#include "potiongraphicsitem.h"
#include "enemygraphicsitem.h"

#include "Config.h"
#include <QGraphicsScene>

GraphicsManager::GraphicsManager() {
    auto w_instance = WorldInstance::getInstance();
    _gridSize = Config::TILE_SIZE;
    qDebug() << "graphicsManager constructor";

}

GraphicsManager::~GraphicsManager()
{
    qDebug() << "graphicsManager Destroyed";
}

void GraphicsManager::init()
{
    auto w_instance = WorldInstance::getInstance();
    if (!w_instance)
    {
        qDebug() << "WorldInstance empty: Could not populate!!!";
        return;
    }

    // GAME MAP:
    _mapGraphicsItem = new MapGraphicsItem(w_instance->getRows(),
                                                     w_instance->getCols(),
                                                     w_instance->getTiles());
    _mapGraphicsItem->setZValue(0);
    _scene->addItem(_mapGraphicsItem);

    // PROTAGONIST
    ProtagonistGraphics* protagonistGraphics = new ProtagonistGraphics();
    protagonistGraphics->setZValue(2);
    _graphicsitems.emplace(w_instance->getProtagonist().get(), std::move(protagonistGraphics));
    protagonistGraphics->setFlag(QGraphicsItem::ItemIsFocusable);
    protagonistGraphics->setFocus();
    _scene->addItem(protagonistGraphics);

    // POTIONS
    for (auto& potionPtr : w_instance->getHealthPotions()) {
        PotionGraphicsItem* potionGraphicsItem = new PotionGraphicsItem(potionPtr->getXPos(),
                                                                    potionPtr->getYPos());
        potionGraphicsItem->setZValue(1);
        _graphicsitems.emplace(potionPtr.get(), std::move(potionGraphicsItem));
        _scene->addItem(potionGraphicsItem);
        qDebug() << "potion added";
    }

    // ENEMIES
    for (auto& enemPtr : w_instance->getEnemies()) {
        EnemyGraphicsItem* enemyGraphicsItem = new EnemyGraphicsItem(enemPtr->getXPos(),
                                                                    enemPtr->getYPos(),
                                                                    100);
        enemyGraphicsItem->setZValue(1);
        _graphicsitems.emplace(enemPtr.get(), std::move(enemyGraphicsItem));
        _scene->addItem(enemyGraphicsItem);
    }
    qDebug() << "GameGraphView: components added";
}

void GraphicsManager::update()
{

}

void GraphicsManager::destroy()
{

}

void GraphicsManager::removeGraphics(Tile* itemToRemove)
{
    auto it = _graphicsitems.find(itemToRemove);
    if (it != _graphicsitems.end()) {
        if (_scene) {
            _scene->removeItem(it->second);
        }
        _graphicsitems.erase(it);
    }
}

void GraphicsManager::updateRenderedEntities() {
    auto w_instance = WorldInstance::getInstance();
    int char_x = w_instance->getProtagonist()->getXPos();
    int char_y = w_instance->getProtagonist()->getYPos();
    int colStart = char_x - Config::RENDER_START_OFFSET;
    int colEnd = char_x + Config::RENDER_END_OFFSET;
    int rowStart = char_y - Config::RENDER_START_OFFSET;
    int rowEnd = char_y + Config::RENDER_END_OFFSET;
    qDebug() << "rendered entities checking...";
    for (int i = colStart; i < colEnd; ++i) {
        for (int j = rowStart; j < rowEnd; ++j) {
            int index = j * _columns + i;
            if (i < _rows && j < _columns && i >= 0 && j >= 0) {
                if (w_instance->getTiles()[index]) {
                    // This tile is in the rendered area
                    // Set isItemInRenderedArea to true for all entities at this tile
                    for (auto& entityPair : isItemInRenderedArea) {
                        if (entityPair.first->pos().x() == i && entityPair.first->pos().y() == j) {
                            entityPair.second.store(true);
                        }
                    }
                } else {
                    // This tile is not in the rendered area
                    // Set isItemInRenderedArea to false for all entities at this tile
                    for (auto& entityPair : isItemInRenderedArea) {
                        if (entityPair.first->pos().x() == i && entityPair.first->pos().y() == j) {
                            entityPair.second.store(false);
                        }
                    }
                }
            }
        }
    }
    qDebug() << "Item In rendered area" << isItemInRenderedArea;
}

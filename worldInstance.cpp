#include "worldInstance.h"
#include "Config.h"
// #include "graphicsManager.h"
#include "animationmanager.h"
#include <iostream>
#include <stdexcept>
#include <QDebug>
#include <random>
#include <thread>
#include <atomic>
#include <future>
#include <QVector2D>
#include "astar.h"
#include "node.h"

WorldInstance::WorldInstance(QObject *parent)
    : QObject(parent),
    _protagonist(std::move(_world->getProtagonist()))
// _graphicsManager(std::make_unique<graphicsManager>())
{
    _gridSize = Config::TILE_SIZE;
    setAtomicKeyAction(false);
    setAtomicPathAction(false);
    _protagonistAttack = false;
    // connect(this, &WorldInstance::mouseUpdate, this, &WorldInstance::updatePathfinder);
    qDebug() << "WorldInstance constructor";
}

void WorldInstance::pushActionToStack(ActionType action) {
    // std::scoped_lock lock(_mutex, _mutex2);
    _keyActionQueue.push(action);
    // lock.unlock();
    _condition.notify_one();
    qDebug() << "WorldController stack size" << _keyActionQueue.size();
}

void WorldInstance::startGameLoop() {
    initPosition();
    _animationManager = new AnimationManager();

    // async is separate thread to kill itself after thread detach
    std::cout << "WorldInstance: GameLoop started" << isRunning << std::endl;
    const auto _frameDuration = std::chrono::milliseconds(1000 / Config::FPS);

    std::thread([&, this, _frameDuration]() {
        auto future = std::async(std::launch::async, [&, this, _frameDuration]() {

            while (isRunning) {
                auto frameStart = std::chrono::steady_clock::now();
                if(!_keyActionQueue.empty()){
                    ActionType currentAction = _keyActionQueue.front();
                    setAtomicPathAction(true);
                    if(getAtomicPathAction()){
                        // updatePathfinder();
                        updatePosition();
                    }
                    if (getAtomicKeyAction()){
                        updatePosition();
                    }
                    _animationManager->setAnimation(currentAction);
                    _animationManager->animateAction(currentAction);
                    _keyActionQueue.pop();
                }
                emit(positionUpdated());
                setAtomicKeyAction(false);
                setAtomicPathAction(false);

                // elapsed time in frame
                auto frameEnd = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
                std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
                auto sleepDuration = _frameDuration - elapsed;
                if (sleepDuration > std::chrono::milliseconds(0)) {
                    std::this_thread::sleep_until(frameStart + _frameDuration);
                }
            }

            std::cout << "Thread completed and cleanup. " << isRunning << std::endl;
        });
    }).detach();

    std::cout << "Main thread continues..." << std::endl;
}

void WorldInstance::createWorld(const QString &filename, unsigned int nrOfEnemies, unsigned int nrOfHealthpacks, float pRatio)
{
    try
    {
        // World assets:
        _world = std::make_shared<World>();
        _world->createWorld(filename, nrOfEnemies, nrOfHealthpacks, pRatio);
        _rows = _world->getRows();
        _tiles = _world->getTiles();
        _cols = _world->getCols();

        // Game units:
        _healthPotions = std::move(_world->getHealthPacks());
        for (const auto& potion: _healthPotions){
            Pos2D potionCoords = std::make_pair(potion->getXPos(), potion->getYPos());
            _potionMap.emplace(potionCoords, potion.get());
        }
        _enemies = _world->getEnemies();
        for (const auto& enem: _enemies){
            Pos2D enemyCoords = std::make_pair(enem->getXPos(), enem->getYPos());
            _enemyMap.emplace(enemyCoords, enem.get());
        }
        qDebug() << "Enemy sixe" << _enemyMap.size();
        _enemyDamage = 15;
        _protagonistDamage = 30;
        _potionValue = 20;
        _pathfinderReadyToRender.store(false);
        initializePathfinder();
        // initPosition();
        qDebug() << "WorldInstance: World created";
    }
    catch (const std::logic_error &e)
    {
        qDebug() << "WorldInstance: Error creating world:" << e.what();
    }
}

void WorldInstance::initPosition() {
    auto w_instance = WorldInstance::getInstance();
    isRunning = true;
    // skip walls
    for (const auto& tilePtr : _tiles) {
        int xPos = tilePtr->getXPos();
        int yPos = tilePtr->getYPos();
        float tileValue = tilePtr->getValue();
        if (tileValue != INFINITY && tileValue <  Config::WALL_LIMIT) {
            _availablePositions.emplace_back(xPos, yPos, tileValue);
        }
    }
    // populate occupied pos with x,y enemy and potions
    if (!_availablePositions.empty()) {
        // for (const auto& enemy : _enemies) {
        //     _occupiedPositions.insert({enemy->getXPos(), enemy->getYPos()});
        // }
        for (const auto& pair : _enemyMap){
            _occupiedPositions.insert(pair.first);
        }
        for (const auto& potion : _healthPotions) {
            _occupiedPositions.insert({potion->getXPos(), potion->getYPos()});
        }

        // delete occupied positions
        auto filterOccupied = [&](const auto& pos) {
            return _occupiedPositions.find({std::get<0>(pos), std::get<1>(pos)}) != _occupiedPositions.end();
        };
        auto it = std::remove_if(_availablePositions.begin(), _availablePositions.end(), filterOccupied);
        _availablePositions.erase(it, _availablePositions.end());

        // check for available positions
        if (!_availablePositions.empty()) {
            // random index
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, _availablePositions.size() - 1);
            int randomIndex = distrib(gen);
            w_instance->getProtagonist()->setPos(std::get<0>(_availablePositions[randomIndex]),
                                                 std::get<1>(_availablePositions[randomIndex]));
            emit(positionUpdated());
            qDebug() << "WorldInstance: Protagonist's position set to: (" << std::get<0>(_availablePositions[randomIndex]) << ", " << std::get<1>(_availablePositions[randomIndex]) << ")";
        } else {
            qDebug() << "WorldInstance: No valid positions available for the protagonist.";
        }
    } else {
        qDebug() << "WorldInstance: No available positions in the world.";
    }
}

void WorldInstance::updatePosition()
{
    auto w_instance = WorldInstance::getInstance();
    float velocity = 1.0 / Config::FPS;
    std::unique_lock<std::mutex> lock(_mutex);
    std::queue<ActionType> tempQueue = _keyActionQueue;
    lock.unlock();
    if (!w_instance)
    {
        std::cerr << "w_instance is nullptr" << '\n';
    }
    int nextXPos = w_instance->getProtagonist()->getXPos();
    int nextYPos = w_instance->getProtagonist()->getYPos();

    qDebug() << "updatePosition: Current Position " << nextXPos << " , " << nextYPos;
    qDebug() << "updatePosition: actionQueue size " << _keyActionQueue.size();
    printQueueContents();
    ActionType currentAction;
    try {
        currentAction = tempQueue.front();
        tempQueue.pop();
        _animationManager->setSpriteFrameNb(6);
        switch (currentAction) {
        case ActionType::UP:
            nextYPos -= 1;
            break;
        case ActionType::DOWN:
            nextYPos += 1;
            break;
        case ActionType::LEFT:
            nextXPos -= 1;
            break;
        case ActionType::RIGHT:
            nextXPos += 1;
            break;
        case ActionType::UP_AI:
            nextYPos -= 1;
            break;
        case ActionType::DOWN_AI:
            nextYPos += 1;
            break;
        case ActionType::LEFT_AI:
            nextXPos -= 1;
            break;
        case ActionType::RIGHT_AI:
            nextXPos += 1;
            break;
        case ActionType::UP_RIGHT_AI:
            nextXPos += 1;
            nextYPos -= 1;
            break;
        case ActionType::UP_LEFT_AI:
            nextXPos -= 1;
            nextYPos -= 1;
            break;
        case ActionType::DOWN_RIGHT_AI:
            nextXPos += 1;
            nextYPos += 1;
            break;
        case ActionType::DOWN_LEFT_AI:
            nextXPos -= 1;
            nextYPos += 1;
            break;
        case ActionType::ATTACK:
            _animationManager->setSpriteFrameNb(4);
            _protagonistAttack = true;
            attack();

            break;

        default:
            break;
        }
    } catch (const std::exception& e) {
        qDebug() << "Exception caught: " << e.what();
    }

    if (w_instance->checkMovementPossible(nextXPos, nextYPos)) {
        w_instance->getProtagonist()->setXPos(nextXPos);
        w_instance->getProtagonist()->setYPos(nextYPos);

        // check if potion
        const Pos2D protagonistCoords = std::make_pair(nextXPos, nextYPos);
        auto it = _potionMap.find(protagonistCoords);
        if (it != _potionMap.end()) {
            qDebug() << "Found potion"
                     << it->second->getXPos()
                     << ","
                     << it->second->getXPos();
            // if(_protagonist->getHealth() < 100){
            _protagonist->setHealth(
                std::min(100.0,1.0*(_protagonist->getHealth()+_potionValue)));
            _graphicsManager->removeGraphics(it->second);

            // }
        }
    }

    getNeighboredTiled(std::make_pair(nextXPos, nextYPos));
    setAtomicKeyAction(false);
}

void WorldInstance::goToDestination(int x, int y){
    int currXPos = _protagonist->getXPos();
    int currYPos = _protagonist->getYPos();
    qDebug() << "MAP_PRESS: src: "
             << currXPos << " , "
             << currYPos << " to dest: "
             << x << " , "
             << y;
    initializePathfinder();
    Astar* astarPtr = &getAstar();
    std::vector<Node*> path;
    if(currXPos && currYPos && x && y){

        path = astarPtr->findPath(currXPos, currYPos, x, y);
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            Node* node = *it;

            if (node){
                updateCharacterDirection(node, currXPos, currYPos);
                currXPos = node->getX();
                currYPos = node->getY();
            }
        }
        setAtomicPathAction(true);
    }

    if(path.size()){
        qDebug() << "path found!!" << path.size();
    }
    else qDebug() << "path empty!!" ;

}

void WorldInstance::updateCharacterDirection(Node* node, int& currXPos, int& currYPos)
{
    int dx = node->getX() - currXPos;
    int dy = node->getY() - currYPos;
    auto w_instance = WorldInstance::getInstance();
    if (dx == -1 && dy == -1)
    {
        w_instance->pushActionToStack(WorldInstance::ActionType::UP_LEFT_AI);
    }

    else if (dx == 0 && dy == -1)
    {
        // qDebug() << "MAP_PRESS: Move LEFT";
        w_instance->pushActionToStack(WorldInstance::ActionType::UP_AI);
    }

    else if (dx == 1 && dy == -1)
    {
        // qDebug() << "MAP_PRESS: Move DOWN";
        w_instance->pushActionToStack(WorldInstance::ActionType::UP_RIGHT_AI);
    }

    else if (dx == 1 && dy == 0)
    {
        // qDebug() << "MAP_PRESS: Move UP";
        w_instance->pushActionToStack(WorldInstance::ActionType::RIGHT_AI);
    }

    else if (dx == 1 && dy == 1)
    {
        w_instance->pushActionToStack(WorldInstance::ActionType::DOWN_RIGHT_AI);
    }

    else if (dx == 0 && dy == 1)
    {
        // qDebug() << "MAP_PRESS: Move UP";
        w_instance->pushActionToStack(WorldInstance::ActionType::DOWN_AI);
    }

    else if (dx == -1 && dy == 1)
    {
        // qDebug() << "MAP_PRESS: Move DOWN";
        w_instance->pushActionToStack(WorldInstance::ActionType::DOWN_LEFT_AI);
    }
    else if (dx == -1 && dy == 0)
    {
        // qDebug() << "MAP_PRESS: Move UP";
        w_instance->pushActionToStack(WorldInstance::ActionType::LEFT_AI);
    }
}

int WorldInstance::convert2DTo1D(int xpos, int ypos) const {
    return ypos * _cols + xpos;
}

bool WorldInstance::checkMovementPossible(int x, int y)
{
    if (x < 0 || x >= _cols) return false;
    if (y < 0 || y >= _rows) return false;
    int tileIndex = convert2DTo1D(x,y);

    // walls
    if (_tiles[tileIndex]->getValue() == INFINITY
        || _tiles[tileIndex]->getValue() >  Config::WALL_LIMIT){
        return false;
    }

    const auto it = _enemyMap.find(std::make_pair(x,y));
    if (it != _enemyMap.end()) return false;

    return true;
}

void WorldInstance::attack(){
    WorldInstance::Pos2D playerCoords = std::make_pair(_protagonist->getXPos(),
                                                       _protagonist->getYPos());
    std::vector<WorldInstance::Pos2D> neighbors = getNeighboredTiled(playerCoords);
    // qDebug() << "neighbors :" << neighbors.size();
    // qDebug() << "Enemies :" << _enemyMap.size();
    for (const auto& neighborPos : neighbors) {
        // qDebug() << "neighbors :" << neighborPos.first
        //          << "," << neighborPos.second;
        auto it = _enemyMap.find(neighborPos);
        if (it != _enemyMap.end()) {
            // qDebug() << "neighbors";
            Enemy* enemy = it->second;
            if(_protagonist->getHealth() > 0){
                _protagonist->setHealth(_protagonist->getHealth()+_enemyDamage);
                enemy->setDefeated(true);
                _graphicsManager->removeGraphics(it->second);
                // it->first->setHealth(_protagonist->getHealth()+_enemyDamage);
            }
            std::cout << "Attacking enemy at position (" << enemy->getXPos() << ", "
                      << enemy->getYPos() << ")" << std::endl;
        }
    }

}


std::vector<WorldInstance::Pos2D> WorldInstance::getNeighboredTiled(Pos2D position){
    std::vector<WorldInstance::Pos2D> tiles;
    // qDebug() << "Position player: x " << position.first << "," << position.second;
    // qDebug() << "Protagonist position: x " <<_protagonist->getXPos()
    //          << "," <<_protagonist->getYPos();
    for (int i = std::max(position.first - 1, 0); i <= std::min(position.first + 1, _cols - 1); i++)
    {
        for (int j = std::max(position.second - 1, 0); j <= std::min(position.second + 1, _rows - 1); j++)
        {
            if(i == _protagonist->getXPos()
                && j == _protagonist->getYPos())
                continue;
            else{
                tiles.emplace_back(std::make_pair(i,j));
                // qDebug() << "position neighbor: " <<position.first << "," <<  position.second;
            // }
            }
        }
    }
    qDebug() << "tiles : " << tiles.size();
    return tiles;
}

void WorldInstance::initializePathfinder()
{
    // initialize Astar variables
    _nodes.reserve(_rows * _cols);
    for (auto &t : _tiles) {
        if (t->getValue() == std::numeric_limits<float>::infinity() || t->getValue() >= Config::WALL_LIMIT) {
            _nodes.emplace_back(t->getXPos(), t->getYPos(), 2, 0, 0, nullptr);
        } else {
            _nodes.emplace_back(t->getXPos(), t->getYPos(), t->getValue(), 0, 0,
                                nullptr);
        }
    }
    // std::cout << "node vector not empty supposedly";
    _astar = std::make_unique<Astar>(_nodes, _rows, _cols);
}

// std::ostream &WorldInstance::operator<<(std::ostream &os, const ActionType &obj)
// {
//     // write obj to stream
//     os << "example";
//     return os;
// }







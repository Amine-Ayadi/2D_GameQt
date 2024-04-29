#ifndef WORLDINSTANCE_H
#define WORLDINSTANCE_H

#include <QObject>
#include <QDebug>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <set>
#include "astar.h"
#include "node.h"
#include "world.h"
#include "graphicsManager.h"

class Pathfinder;
class AnimationManager;

// Singleton for accessibility
class WorldInstance : public QObject, public std::enable_shared_from_this<WorldInstance>
{
    Q_OBJECT
public:

    enum class ActionType {
        IDLE,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        ATTACK,
        UP_AI,
        DOWN_AI,
        LEFT_AI,
        RIGHT_AI,
        UP_RIGHT_AI,
        UP_LEFT_AI,
        DOWN_RIGHT_AI,
        DOWN_LEFT_AI,
        NONE,
    };

    typedef std::pair<int, int> Pos2D;

    WorldInstance(const std::shared_ptr<WorldInstance>) = delete;
    static std::shared_ptr<WorldInstance> getInstance() {
        // Use a static function-local shared_ptr to ensure proper destruction
        static std::shared_ptr<WorldInstance> w_instance = std::shared_ptr<WorldInstance>(new WorldInstance());
        return w_instance;
    }
    ~WorldInstance() override = default;
    void startGameLoop();
    void initPosition();
    void createWorld(const QString &filename, unsigned int nrOfEnemies, unsigned int nrOfHealthpacks, float pRatio);
    void pushActionToStack(ActionType action);


    // Getters Setters
    const std::vector<std::pair<int, int>>& getPathfinderCoords() const;
    const std::shared_ptr<World>& getWorld() { return _world; }
    const std::vector<std::unique_ptr<Tile>>& getTiles() const { return _tiles; }
    std::vector<std::unique_ptr<Tile>>& getHealthPotions() { return _healthPotions; }
    std::vector<std::unique_ptr<Enemy>>& getEnemies() { return _enemies; }
    std::unique_ptr<Protagonist>& getProtagonist() { return _protagonist; }
    std::queue<ActionType> getKeyActionQueue() const {return _keyActionQueue;}
    std::queue<ActionType> getPathActionQueue() const {return _pathActionQueue;}

    void setAtomicIsRunning(bool value) {isRunning.store(value);}
    void setAtomicKeyAction(bool value) {_keyAction.store(value);}
    void setAtomicPathAction(bool value) {_PathfinderAction.store(value);}
    void setAtomicPathReady(bool value) {_pathfinderReadyToRender.store(value);}
    bool getAtomicKeyAction() {return _keyAction.load();}
    bool getAtomicPathAction() {return _PathfinderAction.load();}
    bool getAtomicIsRunning() {return isRunning.load();}
    bool getAtomicPathReady() {return _pathfinderReadyToRender.load();}

    int getRows() const { return _rows ;}
    int getCols() const { return _cols ;}
    int getGridSize() const { return _gridSize;}
    void setFramesToRender(int framesToRender) { _framesToRender = framesToRender;}
    int getFramesToRender() const { return _framesToRender;}
    Astar& getAstar() { return *_astar; }

    AnimationManager* animationManager() const { return _animationManager; }
    std::unique_ptr<GraphicsManager>& getGraphicsManager() { return _graphicsManager; }
    void setGraphicsManager(std::unique_ptr<GraphicsManager>& newGraphicsManager) {
        _graphicsManager = std::move(newGraphicsManager);
    }

    const std::mutex& mutex() const { return _mutex; }
    std::mutex& mutex() { return _mutex; }
    const std::mutex& mutex_2() const { return _mutex2; }
    std::mutex& mutex_2() { return _mutex2; }
    std::vector<Node> getNodes() const { return _nodes; }
    QString& getMap() { return _map; }
    void setMap(QString mapSelected) { _map = mapSelected;}

    // METHODS
    void initializePathfinder();
    void getDirection(int currentX, int currentY, int nextX, int nextY);
    int convert2DTo1D(int xpos, int ypos) const;
    bool checkMovementPossible(int x, int y);
    void updateCharacterDirection(Node* node, int& currXPos, int& currYPos);
    void goToDestination(int x, int y);
    void attack();
    std::vector<Pos2D> getNeighboredTiled(Pos2D position);

    // OPERATORS
    // std::ostream& operator<<(std::ostream& os, const ActionType& obj);
    void printQueueContents() {
        // std::scoped_lock<std:mutex> lock(_mutex3);
        std::queue<ActionType> tempQueue = _keyActionQueue;

        while (!tempQueue.empty()) {
            ActionType action = tempQueue.front();

            tempQueue.pop();
            switch(action){
            case ActionType::UP_AI:
                std::cout << "UP_AI" << " ";
                break;
            case ActionType::DOWN_AI:
                std::cout << "DOWN_AI" << " ";
                break;
            case ActionType::LEFT_AI:
                std::cout << "LEFT_AI" << " ";
                break;
            case ActionType::RIGHT_AI:
                std::cout << "RIGHT_AI" << " ";
                break;
            case ActionType::UP:
                std::cout << "UP" << " ";
                break;
            case ActionType::DOWN:
                std::cout << "DOWN" << " ";
                break;
            case ActionType::LEFT:
                std::cout << "LEFT" << " ";
                break;
            case ActionType::RIGHT:
                std::cout << "RIGHT" << " ";
                break;

            default:
                break;
            }


        }
        std::cout << std::endl;
    }

    std::vector<Node> nodes() const;

public slots:
    void updatePosition();
    // void updatePathfinder();

signals:
    void positionUpdated();
    void positionAIUpdated();
    void mouseUpdate();
    void animate();
    void startPressed();
    void mapSelected(const QString &map);
    void exitPressed();
    void protagonistAction();


protected:
    WorldInstance(QObject *parent = nullptr);

private:

    std::shared_ptr<World> _world;
    std::unique_ptr<Protagonist> _protagonist;
    std::vector<std::unique_ptr<Tile>> _tiles;
    std::vector<std::unique_ptr<Tile>> _healthPotions;
    std::vector<std::unique_ptr<Enemy>> _enemies;
    std::set<std::pair<int, int>> _occupiedPositions;
    std::vector<std::tuple<int, int, float>> _availablePositions;
    std::queue<ActionType> _keyActionQueue;
    std::queue<ActionType> _pathActionQueue;
    std::mutex _mutex;
    std::mutex _mutex2;
    std::mutex _mKeyPath;
    std::condition_variable _condition;
    std::condition_variable _condKeyPath;
    std::atomic<bool> isRunning;
    std::atomic<bool> _keyAction;
    std::atomic<bool> _PathfinderAction;
    std::atomic<bool> _protagonistAttack;
    std::atomic<bool> _pathfinderReadyToRender;
    int _enemyDamage;
    int _protagonistDamage;
    int _potionValue;


    float _wallLimit;
    QString _map;
    int _rows;
    int _cols;
    int _gridSize;
    int _framesToRender;
    std::vector<Node> _nodes;
    std::unique_ptr<Astar> _astar;
    QRectF _pixelRect;
    AnimationManager* _animationManager;
    std::unique_ptr<GraphicsManager> _graphicsManager;

    std::map<Pos2D, Enemy*> _enemyMap;
    std::map<Pos2D, Tile*> _potionMap;
    // std::vector<Node*> _nodePtrs;
};

#endif // WORLDINSTANCE_H

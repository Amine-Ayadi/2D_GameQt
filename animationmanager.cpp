#include "animationmanager.h"
#include "worldInstance.h"
#include "Config.h"
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QPropertyAnimation>



AnimationManager::AnimationManager()
    : _currentFrame(0), currentAnimation(WorldInstance::ActionType::IDLE)
{
    qDebug() << "AnimationManager constructor";
    auto w_instance = WorldInstance::getInstance();
    // load spritesheets
    _spriteSheets[WorldInstance::ActionType::RIGHT] = QPixmap(":/images/right_sprite.png");
    _spriteSheets[WorldInstance::ActionType::LEFT] = QPixmap(":/images/left_sprite.png");
    _spriteSheets[WorldInstance::ActionType::UP] = QPixmap(":/images/up_sprite.png");
    _spriteSheets[WorldInstance::ActionType::DOWN] = QPixmap(":/images/down_sprite.png");
    _spriteSheets[WorldInstance::ActionType::RIGHT_AI] = QPixmap(":/images/right_sprite.png");
    _spriteSheets[WorldInstance::ActionType::LEFT_AI] = QPixmap(":/images/left_sprite.png");
    _spriteSheets[WorldInstance::ActionType::UP_AI] = QPixmap(":/images/up_sprite.png");
    _spriteSheets[WorldInstance::ActionType::DOWN_AI] = QPixmap(":/images/down_sprite.png");
    _spriteSheets[WorldInstance::ActionType::IDLE] = QPixmap(":/images/idlebt_sprite.png");
    _spriteSheets[WorldInstance::ActionType::ATTACK] = QPixmap(":/images/attack_down.png");
    _spriteSheets[WorldInstance::ActionType::UP_RIGHT_AI] = QPixmap(":/images/up_sprite.png");
    _spriteSheets[WorldInstance::ActionType::UP_LEFT_AI] = QPixmap(":/images/up_sprite.png");
    _spriteSheets[WorldInstance::ActionType::DOWN_RIGHT_AI] = QPixmap(":/images/down_sprite.png");
    _spriteSheets[WorldInstance::ActionType::DOWN_LEFT_AI] = QPixmap(":/images/down_sprite.png");
    setAnimation(WorldInstance::ActionType::IDLE);

    // w_instance->setFramesToRender(_spriteSheets[currentAnimation].width() / 24);
    _nbSpriteFrames = 6;
    _currentFrame = 0;
    _frameWidth = _spriteSheets[currentAnimation].width() / _nbSpriteFrames;
    _frameHeight = _spriteSheets[currentAnimation].height();
}

QRectF AnimationManager::boundingRect() const
{
    // THIS CLASS SHOULDN T BE A QGRAPHICSITEM
}

AnimationManager::~AnimationManager()
{
}

void AnimationManager::nextFrame()
{
    // qDebug() << "RESULT:" << (_currentFrame + 1) % _spriteSheets[currentAnimation].width();
    if(_currentFrame < 6){
        try {
            if (_spriteSheets[currentAnimation].width() == 0) {
                throw std::runtime_error("Width of sprite sheet is zero");
            }
            _currentFrame = (_currentFrame + 1) % _spriteSheets[currentAnimation].width();
            qDebug() << "_currentFrame: " << _currentFrame;
        } catch (const std::exception& e) {
            qDebug() << "Exception caught: " << e.what();
        }

        update();
    }
    else{
        _currentFrame = 0;
    }

}

void AnimationManager::updateAnimation(int elapsedFrames)
{
    // auto w_instance = WorldInstance::getInstance();
    // std::lock_guard<std::mutex> lock(w_instance->mutex());
    int framesToAdvance = elapsedFrames / (1000 / Config::FPS); // 1000 ms / FPS
    //    qDebug() << "frameToAdvance : "<< framesToAdvance;
    _currentFrame += framesToAdvance;

    // loop back if end reached
    if (_currentFrame >= _spriteSheets[currentAnimation].width()) {
        _currentFrame = 0;
    }
    update();
}

QPixmap AnimationManager::getSpritePixmap(int index) const
{
    // auto w_instance = WorldInstance::getInstance();
    // std::lock_guard<std::mutex> lock(w_instance->mutex());
    QMap<WorldInstance::ActionType, QPixmap>::const_iterator it = _spriteSheets.begin();
    std::advance(it, index);
    if (it != _spriteSheets.end())
    {
        // qDebug() << "AnimationManager::getSpriteSheet value: " << it.value();
        return it.value();
    }
    else
    {
        return QPixmap();
    }
}

QPixmap AnimationManager::getPixmapFromFrame(int currentFrame) const
{
    // auto w_instance = WorldInstance::getInstance();
    // std::lock_guard<std::mutex> lock(w_instance->mutex());
    if(currentFrame<6){
        // position in spritesheet
        // qDebug() << "AnimationManager::currentFrame : " << currentFrame;
        int x = currentFrame * _frameWidth;
        int y = 0;

        // frame window
        QRect frameRect(x, y, _frameWidth, _frameHeight);
        return _spriteSheets[currentAnimation].copy(frameRect);

    } else {
        int x = 0;
        int y = 0;
        QRect frameRect(x, y, _frameWidth, _frameHeight);
        return _spriteSheets[currentAnimation].copy(frameRect);
    }
}

void AnimationManager::animateAction(WorldInstance::ActionType action)
{
    Q_UNUSED(action)
    auto w_instance = WorldInstance::getInstance();
    std::lock_guard<std::mutex> lock(w_instance->mutex());
    nextFrame();
    // updateAnimation(elapsedFrames);
    // qDebug() << "frame count : "<< w_instance->getFramesToRender();
    // updateAnimation(1000 / Config::FPS);
    // update();
}

void AnimationManager::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    Q_UNUSED(painter)
}

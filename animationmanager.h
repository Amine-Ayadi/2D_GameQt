#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QGraphicsItem>
#include "worldInstance.h"


class AnimationManager : public QGraphicsItem
{
public:
    AnimationManager();
    ~AnimationManager() override;

    std::map<WorldInstance::ActionType, std::string> animationNames = {
        {WorldInstance::ActionType::RIGHT, "right"},
        {WorldInstance::ActionType::LEFT, "left"},
        {WorldInstance::ActionType::UP, "up"},
        {WorldInstance::ActionType::DOWN, "down"},
        {WorldInstance::ActionType::IDLE, "idle"},
        {WorldInstance::ActionType::ATTACK, "attack"},
        {WorldInstance::ActionType::UP_RIGHT_AI, "upRight"},
        {WorldInstance::ActionType::UP_LEFT_AI, "upLeft"},
        {WorldInstance::ActionType::DOWN_RIGHT_AI, "downRight"},
        {WorldInstance::ActionType::DOWN_LEFT_AI, "downLeft"}
    };

    QRectF boundingRect() const override;
    QPixmap getSpritePixmap(int index) const;
    QPixmap getPixmapFromFrame(int index) const;
    void nextFrame();
    void updateAnimation(int elapsedFrames);
    void animateAction(WorldInstance::ActionType action);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    void setAnimation(WorldInstance::ActionType animation){ currentAnimation = animation;}
    WorldInstance::ActionType getCurrentAnimation(){ return currentAnimation;}
    int getCurrentFrame(){ return _currentFrame;}
    float getSpriteFrameNb() { return _nbSpriteFrames; }
    void setSpriteFrameNb(float nbFrames) { _nbSpriteFrames = nbFrames;}

    QMap<WorldInstance::ActionType, QPixmap> spriteSheets() const;

private:
    int _currentFrame;
    int _frameHeight;
    int _frameWidth;
    float _nbSpriteFrames;
    WorldInstance::ActionType currentAnimation;
    QMap<WorldInstance::ActionType, QPixmap> _spriteSheets;

};

#endif // ANIMATIONMANAGER_H

#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include "gamegraphview.h"
#include "gamescreen.h"

#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QObject>

class GameController;
class QGraphicsScene;
class WorldInstance;
class ProtagonistGraphics;
class QSlider;
class QLabel;
class QMainWindow;
class CustomBtScene;

class WelcomeScreen : public GameScreen//: public GameGraphView
{
    Q_OBJECT
public:
    explicit WelcomeScreen(const QString &name, QWidget *parent = nullptr);
    ~WelcomeScreen() override;
    QGraphicsView *view() const override;
    void populateScene() override;
    void setScene(QGraphicsScene *newScene) override {this->_scene = newScene;}
    QGraphicsScene *getScene() const override { return _scene; }

    void buildScreen();
    const QString getMap(int index) const;

private:
    QGraphicsView* _view;
    QGraphicsScene* _scene;
    CustomBtScene* _startBt;
    CustomBtScene* _exitBt;
    QMap<int, QString> _maps;

};



#endif // WELCOMESCREEN_H

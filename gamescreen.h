#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <QFrame>

class QGraphicsView;
class QGraphicsScene;

class GameScreen : public QFrame
{
    Q_OBJECT

public:
    explicit GameScreen(const QString &name, QWidget *parent = nullptr);
    virtual ~GameScreen() = 0; // Pure virtual destructor
    virtual QGraphicsView *view() const;
    virtual void populateScene() = 0;
    virtual QGraphicsScene *getScene() const = 0;
    virtual void setScene(QGraphicsScene *newScene) = 0;

private:
    QGraphicsView* _view;
};




#endif // GAMESCREEN_H

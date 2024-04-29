#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QKeyEvent>

class GameController : public QObject
{
    Q_OBJECT
public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

signals:
    void showOptionsRequested();
    void addTextConsole(const QString& message);
    void keyPressRequested(QKeyEvent *event);

public slots:
    void handleShowOptions();
    void handleSelectedTile();
};

#endif // GAMECONTROLLER_H

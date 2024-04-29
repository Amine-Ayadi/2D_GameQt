#ifndef SCENEEVENTHANDLER_H
#define SCENEEVENTHANDLER_H

#include <QObject>

class SceneEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit SceneEventHandler(QObject *parent = nullptr);

signals:
    void clicked();

};

#endif // SCENEEVENTHANDLER_H

#ifndef ASTAR_H_
#define ASTAR_H_
#include <QObject>
#include <vector>
#include <queue>
#include "node.h"

class Astar : public QObject
{
    Q_OBJECT
public:
    Astar(std::vector<Node> nds, int rs, int cls);
    std::vector<Node *> findPath(int sX, int sY, int dX, int dY);
    void resetMap();
    void updateWeight(int x,int y,float w);
    void updateFlag(int x, int y, char f);

public slots:
    void heuristicChanged(float newHeuristic) {m_heuristicWeight = newHeuristic/10;}
    void pathWeightChanged(float newPathWeight) {m_pathWeight = newPathWeight;}

private:
    std::vector<Node>nodes;
    int rows;
    int columns;
    Node * last_node;

    float m_heuristicWeight = 0;
    float m_pathWeight = 0;
};
#endif

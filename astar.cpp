#include "astar.h"
#include "comparator.h"
#include <iostream>
#include <cmath>

/* ME
#ifdef DEBUG
#define PRINT(str) std::cout << str << std::endl
#else
#define PRINT(str)
#endif
*/

// parameter nds vector of nodes, created in the main and then passed to the Astar class, which will keep them
// until the end of its lifetime. So only one copy is made.
// parameter rs number of rows
// parameter cls  number of columns
Astar::Astar(std::vector<Node>nds, int rs, int cls) : nodes{nds}, rows{rs},
                                                        columns{cls}
{
}
inline float ChebyshevDistance(int sx, int sy, int gx, int gy)
{
    return (std::max(std::abs(gx - sx),
                          std::abs(gy - sy)));
}

/*
step 1: initialize solution, priority queue (pq)
step 2: add the start node to the pq
step 2: while pq is not empty
step 3: pop the pq
step 4: set the popped to Done
check : check if we are at destination, if we are populate the solution vector
step 6: for each succesor, which are the basically the neighbors
step 7: calculate the distance to the neighbor,
step 8: calculate the cost to travel to that neighbor
check : check if the node is done, if it is continue with the next neighbor
check : check if the node is visited, if it is not add it to the frontier and update its fields
check : if node is visited and the calculated cost for the same node is lower,
update its fields and add it to the frontier again
parameter sX source X
parameter sY source U
parameter dX destination X, same as goal X
parameter dY destination Y, same as goal Y
parameter pw path weight from the ui
parameter hw heuristic weight from the ui
return vector containing the solution nodes
*/
std::vector<Node *> Astar::findPath(int sX, int sY, int dX, int dY)
{
    float pw = m_pathWeight;
    float hw = m_heuristicWeight;
    std::priority_queue<Node*, std::vector<Node*>, comparator> frontier;

    std::vector<Node *> solution;

    solution.reserve(std::sqrt(pow(rows, 2) + pow(columns, 2)));

    int start_index = columns * sY+ sX;
    nodes[start_index].setCost(0);
    nodes[start_index].setLength(0);
    nodes[start_index].setHeuristic(0);
    nodes[start_index].setParent(nullptr);
    frontier.push(&nodes[start_index]);

    while (!frontier.empty())
    {
        auto currNode = frontier.top();
        frontier.pop();
        if(currNode->getFlag() != 3){
            currNode->setFlag(2);
        }
        int curr_x = currNode->getX();
        int curr_y = currNode->getY();
        float curr_cost = currNode->getCost();
        int curr_length = currNode->getLength();

        if (curr_x == dX && curr_y == dY)
        {
            auto n = currNode;
            while (true)
            {
                solution.push_back(n);
                n = n->getParent();
                if (n == nullptr)
                {
                    break;
                }
            }
            return solution;
        }
        // for each successor
        for (int i = std::max(curr_x - 1, 0); i <= std::min(curr_x + 1, columns - 1); i++)
        {
            for (int j = std::max(curr_y - 1, 0); j <= std::min(curr_y + 1, rows - 1); j++)
            {
                bool add = false;
                int index = i + columns * j;
                float succ_w = nodes[index].getW();
                // 2 is wall
                if(succ_w == 2){
                    continue;
                }
                if (nodes[index].getFlag() == 2 || nodes[index].getFlag() == 4)
                {
                    continue;
                }
                if (nodes[index].getFlag() == 3)
                {
                    if(i==dX && j == dY){
                        nodes[index].setFlag(4);
                        add = true;
                    }else{
                        continue;
                    }
                }
                int distance = std::abs(i-curr_x) + std::abs(j-curr_y);
                float cost = curr_cost+(1-succ_w);//*sqrt(distance);
                float newHeuristic =  cost + hw*ChebyshevDistance(i,j,dX,dY) + pw*(curr_length+distance);
                if (nodes[index].getFlag()  == 0 )
                {
                    nodes[index].setFlag(1);
                    add = true;
                }
                else if (cost < nodes[index].getCost())
                {
                    add = true;
                }
                if(add){
                    nodes[index].setHeuristic(newHeuristic);
                    nodes[index].setCost(cost);
                    nodes[index].setLength(curr_length+distance);
                    nodes[index].setParent(currNode);
                    frontier.push(&nodes[index]);
                }
            }
        }
    }
    return solution;
}
// reset the cost, done, and visited fields of the nodes in the map
void Astar::resetMap()
{
    for (auto &n : nodes)
    {
        if(n.getFlag()==4){
            n.resetSpecialDone();
        }
        else if(n.getFlag()==3){
            n.resetSpecial();
        }else{
            n.resetTile();
        }
    }
}

void Astar::updateWeight(int x, int y, float w)
{
    nodes[y*columns+x].setW(w);

}
void Astar::updateFlag(int x, int y, char f)
{
    nodes[y*columns+x].setFlag(f);

}

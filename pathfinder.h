#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <unordered_map>
#include <set>
#include <cmath>
#include "worldInstance.h"

class Pathfinder {
public:
    Pathfinder();
    std::vector<std::pair<int, int>> findPath(int xStart, int yStart, int xEnd, int yEnd);

private:

    struct Node {
        int x, y;
        float f, g, h;
        Node* parent;

        Node(int x, int y) : x(x), y(y), f(0), g(0), h(0), parent(nullptr) {}
        bool operator<(const Node& other) const {
            // Compare nodes based on their f-value
            return f < other.f;
        }
    };

    std::vector<std::pair<int, int>> reconstructPath(Node* current);
    float heuristic(int x1, int y1, int x2, int y2);
};

#endif // PATHFINDER_H


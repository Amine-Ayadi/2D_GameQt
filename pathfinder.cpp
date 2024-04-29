#include "pathfinder.h"

Pathfinder::Pathfinder() {

}

std::vector<std::pair<int, int>> Pathfinder::findPath(int xStart, int yStart, int xEnd, int yEnd) {
    // Intantiate world
    auto w_instance = WorldInstance::getInstance();

    // Initialize the open and closed sets
    std::set<Node> openSet;
    std::unordered_map<int, Node*> openMap;
    std::unordered_map<int, Node*> closedMap;

    // Create start and end nodes
    Node start(xStart, yStart);
    Node end(xEnd, yEnd);

    // Add the start node to the open set
    openSet.insert(start);
    openMap[yStart * w_instance->getCols() + xStart] = &start;

    while (!openSet.empty()) {
        // Get the node with the lowest f-value from the open set
        Node current = *openSet.begin();
        openSet.erase(openSet.begin());
        openMap.erase(current.y * w_instance->getCols() + current.x);

        // Add the current node to the closed set
        closedMap[current.y * w_instance->getCols() + current.x] = &current;

        // Check if we've reached the end node
        if (current.x == xEnd && current.y == yEnd) {
            return reconstructPath(&current);
        }

        // Explore neighboring nodes
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) {
                    continue;  // Skip the current node
                }

                int neighborX = current.x + dx;
                int neighborY = current.y + dy;

                // Check if the neighbor is within the world bounds
                if (neighborX < 0 || neighborX >= w_instance->getCols() || neighborY < 0 || neighborY >= w_instance->getRows()) {
                    continue;
                }

                // Calculate neighbor's index for the map
                int neighborIndex = neighborY * w_instance->getCols() + neighborX;

                // Check if the neighbor tile is impassable (value == infinity)
                if (std::isinf(w_instance->getTiles()[neighborIndex]->getValue())) {
                    continue;
                }

                // Create the neighbor node
                Node neighbor(neighborX, neighborY);

                // Calculate tentative g value
                float tentativeG = current.g + 1.0f;

                // Check if the neighbor is already in the closed set with a lower g value
                if (closedMap.find(neighborIndex) != closedMap.end() && tentativeG >= closedMap[neighborIndex]->g) {
                    continue;
                }

                // Check if the neighbor is not in the open set or has a lower g value
                if (openMap.find(neighborIndex) == openMap.end() || tentativeG < openMap[neighborIndex]->g) {
                    // Update neighbor's values
                    neighbor.parent = &current;
                    neighbor.g = tentativeG;
                    neighbor.h = heuristic(neighborX, neighborY, xEnd, yEnd);
                    neighbor.f = neighbor.g + neighbor.h;

                    // Add the neighbor to the open set
                    openSet.insert(neighbor);
                    openMap[neighborIndex] = &neighbor;
                }
            }
        }
    }

    // No path found
    return std::vector<std::pair<int, int>>();
}

std::vector<std::pair<int, int>> Pathfinder::reconstructPath(Node* current) {
    std::vector<std::pair<int, int>> path;

    while (current != nullptr) {
        path.emplace_back(current->x, current->y);
        current = current->parent;
    }

    // Reverse the path to start from the start node and end at the goal node
    std::reverse(path.begin(), path.end());

    return path;
}


float Pathfinder::heuristic(int x1, int y1, int x2, int y2) {
    // I chose Manhanttan distance since it is a grid based game where protagonsit moves only up, down, left and right
    // I used it for Pacman game(very similar to this pathfinder game) in Python for my AI course and it worked well
    // Calculate the heuristic (estimated) cost between two points
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

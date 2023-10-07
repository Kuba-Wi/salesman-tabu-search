#include "pathFinder.hpp"

#include <iostream>

int main() {
    PathFinder pathFinder;
    pathFinder.readDistancesFromFile("../distances2.txt");
    auto bestPath = pathFinder.findBestPath();
    auto length = pathFinder.getPathLength(bestPath);
    for (auto node : bestPath) {
        std::cout << node << " ";
    }
    std::cout << "\n";
    std::cout << length << "\n";
}

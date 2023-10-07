#include "pathFinder.hpp"

#include <iostream>

int main() {
    PathFinder pathFinder;
    pathFinder.readDistancesFromFile("distances.txt");
    auto best = pathFinder.findBestPath();
    auto length = pathFinder.getPathLength(best);
    for (auto node : best) {
        std::cout << node << " ";
    }
    std::cout << "\n";
    std::cout << length << "\n";
}

#include "pathFinder.hpp"
#include "geneticFinder.hpp"

#include <iostream>

int main() {
    PathFinder pathFinder;
    pathFinder.readDistancesFromFile("../distances2.txt");
    auto bestPath = pathFinder.findBestPath();
    for (auto node : bestPath) {
        std::cout << node << " ";
    }
    std::cout << "\n";
    std::cout << pathFinder.getPathLength(bestPath) << "\n";

    GeneticFinder geneticFinder;
    geneticFinder.readDistancesFromFile("../distances2.txt");
    bestPath = geneticFinder.findBestPath();
    for (auto node : bestPath) {
        std::cout << node << " ";
    }
    std::cout << "\n";
    std::cout << geneticFinder.getPathLength(bestPath) << "\n";
}

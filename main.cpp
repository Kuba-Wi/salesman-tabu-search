#include "pathFinder.hpp"
#include "geneticFinder.hpp"

#include <chrono>
#include <iostream>

int main() {
    PathFinder pathFinder;
    pathFinder.readDistancesFromFile("../distances2.txt");
    auto begin = std::chrono::steady_clock::now();
    auto bestPath = pathFinder.findBestPath();
    auto end = std::chrono::steady_clock::now();
    for (auto node : bestPath) {
        std::cout << node << " ";
    }
    std::cout << "\n";
    std::cout << pathFinder.getPathLength(bestPath) << "\n";
    std::cout << "Path finder execution time = " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() 
              << " [µs]\n\n";

    GeneticFinder geneticFinder;
    geneticFinder.readDistancesFromFile("../distances2.txt");
    begin = std::chrono::steady_clock::now();
    bestPath = geneticFinder.findBestPath();
    end = std::chrono::steady_clock::now();
    for (auto node : bestPath) {
        std::cout << node << " ";
    }
    std::cout << "\n";
    std::cout << geneticFinder.getPathLength(bestPath) << "\n";
    std::cout << "Genetic finder execution time = " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() 
              << " [µs]\n";
}

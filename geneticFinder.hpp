#pragma once

#include <mutex>
#include <string>
#include <thread>
#include <vector>

class GeneticFinder {
public:
    void readDistancesFromFile(const std::string& filename);
    std::vector<size_t> findBestPath();
    size_t getPathLength(const std::vector<size_t>& path) const;

private:
    void generateInitialPopulation();
    void generateInitialPath(std::vector<size_t>& path) const;
    bool isNodeInPath(const std::vector<size_t>& path, size_t node) const;
    std::vector<size_t> crossPaths(const std::vector<size_t>& first, const std::vector<size_t>& second) const;
    void mutatePath(std::vector<size_t>& path) const;
    void orderPopulation();
    void crossPathsThreadFun(std::vector<std::vector<size_t>>& newPopulation, size_t begin, size_t end) const;

    std::vector<std::vector<size_t>> distances_;
    std::vector<std::vector<size_t>> population_;
    std::vector<std::thread> threadsVector_;
    mutable std::mutex randMx_;
};

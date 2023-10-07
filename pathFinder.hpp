#pragma once

#include <string>
#include <thread>
#include <vector>

class PathFinder {
public:
    void readDistancesFromFile(const std::string& filename);
    size_t getPathLength(const std::vector<size_t>& path);
    std::vector<size_t> findBestPath();

private:
    bool isOnTabuList(const std::vector<std::pair<int, int>>& tabuList, const std::pair<int, int>& node) const;
    std::pair<int, int> getBestNeighbour(std::vector<size_t>& path, const std::vector<std::pair<int, int>>& tabuList);
    void updateTabuList(std::vector<std::pair<int, int>>& tabuList, const std::pair<int, int>& value, size_t& tabuCurrentIndex);
    void cleanTabuList(std::vector<std::pair<int, int>>& tabuList, size_t& tabuCurrentIndex);
    void generateInitialPath(std::vector<size_t>& path);
    bool isNodeInPath(const std::vector<size_t>& path, size_t node) const;
    void findBestPathThreadFunction(std::vector<size_t>& path);

    const size_t tabuListSize_ = 7;
    const size_t iterationsCount_ = 100;
    const std::pair<int, int> emptyTabuFieldValue_{-1, -1};
    const size_t threadsCount_ = 5;

    std::vector<std::vector<size_t>> distances_;
    std::vector<std::thread> threadsVector_;
};

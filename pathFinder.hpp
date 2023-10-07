#pragma once

#include <string>
#include <vector>

class PathFinder {
public:
    PathFinder();
    void readDistancesFromFile(const std::string& filename);
    size_t getPathLength(const std::vector<size_t>& path);
    std::vector<size_t> findBestPath();

private:
    bool isOnTabuList(const std::pair<int, int>& node);
    std::pair<int, int> getBestNeighbour(std::vector<size_t>& path);
    void updateTabuList(const std::pair<int, int>& value);
    void cleanTabuList();

    const size_t tabuListSize_ = 7;
    const size_t iterationsCount_ = 100;
    const int emptyTabuFieldValue_ = -1;

    std::vector<std::vector<size_t>> distances_;
    std::vector<std::pair<int, int>> tabuList_;
    size_t tabuCurrentIndex_ = 0;
};

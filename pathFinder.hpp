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
    void getBestNeighbourThreadFunction(std::vector<size_t> path, 
                                        const std::vector<std::pair<int, int>>& tabuList, 
                                        size_t firstNode,
                                        size_t lastNode,
                                        std::pair<int, int>& bestNeighbour,
                                        size_t& bestNeighbourLength);

    void updateTabuList(std::vector<std::pair<int, int>>& tabuList, const std::pair<int, int>& value, size_t& tabuCurrentIndex);
    void cleanTabuList(std::vector<std::pair<int, int>>& tabuList, size_t& tabuCurrentIndex);
    void generateInitialPath(std::vector<size_t>& path);
    bool isNodeInPath(const std::vector<size_t>& path, size_t node) const;
    void findBestPathThreadFunction(std::vector<size_t>& path);

    const std::pair<int, int> EMPTY_NEIGHBOUR_{-1, -1};
    const size_t NEIGHBOUR_THREADS_ = 5;
    const size_t THREADS_COUNT_ = 5;
    const size_t TABU_LIST_SIZE_ = 7;

    std::vector<std::vector<size_t>> distances_;
    std::vector<std::thread> threadsVector_;
};

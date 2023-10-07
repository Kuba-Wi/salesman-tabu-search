#include "pathFinder.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

void PathFinder::readDistancesFromFile(const std::string& filename) {
    std::ifstream input;
    input.open(filename);
    if (!input.is_open()) {
        std::cerr << "Could not read file with distances" << std::endl;
        return;
    }

    std::string line;
    std::istringstream lineStream;
    size_t tmp;
    while (std::getline(input, line)) {
        lineStream.clear();
        lineStream.str(line);
        distances_.emplace_back(std::vector<size_t>{});
        while (lineStream >> tmp) {
            distances_.back().emplace_back(tmp);
        }
    }
}

size_t PathFinder::getPathLength(const std::vector<size_t>& path) {
    size_t length = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        length += distances_[path[i]][path[i + 1]];
    }
    length += distances_[path.front()][path.back()];
    return length;
}

bool PathFinder::isOnTabuList(const std::vector<std::pair<int, int>>& tabuList, const std::pair<int, int>& node) const {
    return std::any_of(tabuList.begin(), tabuList.end(), [&](auto& el){
        return el == node;
    });
}

std::pair<int, int> PathFinder::getBestNeighbour(std::vector<size_t>& path, const std::vector<std::pair<int, int>>& tabuList) {
    std::pair<int, int> bestNeighbour = emptyTabuFieldValue_;
    size_t bestNeighbourLength = std::numeric_limits<size_t>::max();
    size_t tmp_length = 0;
    for (size_t first = 1; first < path.size() - 1; ++first) {
        for (size_t second = first + 1; second < path.size(); ++second) {
            if (this->isOnTabuList(tabuList, {first, second})) {
                continue;
            }
            std::swap(path[first], path[second]);
            tmp_length = this->getPathLength(path);
            if (tmp_length < bestNeighbourLength) {
                bestNeighbourLength = tmp_length;
                bestNeighbour.first = first;
                bestNeighbour.second = second;
            }
            std::swap(path[first], path[second]);
        }
    }
    return bestNeighbour;
}

void PathFinder::updateTabuList(std::vector<std::pair<int, int>>& tabuList, const std::pair<int, int>& value, size_t& tabuCurrentIndex) {
    tabuList[tabuCurrentIndex] = value;
    ++tabuCurrentIndex;
    if (tabuCurrentIndex >= tabuList.size()) {
        tabuCurrentIndex = 0;
    }
}

void PathFinder::cleanTabuList(std::vector<std::pair<int, int>>& tabuList, size_t& tabuCurrentIndex) {
    for (auto& el : tabuList) {
        el = emptyTabuFieldValue_;
    }
    tabuCurrentIndex = 0;
}

void PathFinder::generateInitialPath(std::vector<size_t>& path) {
    std::srand(std::time(nullptr));
    size_t nodeIndex = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        while (this->isNodeInPath(path, nodeIndex)) {
            nodeIndex = std::rand() % path.size();
        }
        path[i] = nodeIndex;
    }
}

bool PathFinder::isNodeInPath(const std::vector<size_t>& path, size_t node) const {
    return std::any_of(path.begin(), path.end(), [&](auto value){
        return value == node;
    });
}

void PathFinder::findBestPathThreadFunction(std::vector<size_t>& path) {
    std::vector<std::pair<int, int>> tabuList(tabuListSize_, emptyTabuFieldValue_);
    size_t tabuCurrentIndex = 0;
    this->generateInitialPath(path);
    auto bestPath = path;
    size_t pathLength = this->getPathLength(path);
    size_t bestPathLength = pathLength;

    std::pair<int, int> bestNeighbour;
    for (size_t i = 0; i < iterationsCount_ / threadsCount_; ++i) {
        bestNeighbour = this->getBestNeighbour(path, tabuList);
        if (bestNeighbour != emptyTabuFieldValue_) {
            std::swap(path[bestNeighbour.first], path[bestNeighbour.second]);
            this->updateTabuList(tabuList, bestNeighbour, tabuCurrentIndex);
            pathLength = this->getPathLength(path);
            if (pathLength < bestPathLength) {
                bestPathLength = pathLength;
                bestPath = path;
            }
        } else {
            this->cleanTabuList(tabuList, tabuCurrentIndex);
        }
    }
    path = bestPath;
}

std::vector<size_t> PathFinder::findBestPath() {
    threadsVector_.clear();
    std::vector<std::vector<size_t>> paths(threadsCount_, std::vector<size_t>(distances_.size(), 0));
    for (size_t i = 0; i < threadsCount_; ++i) {
        threadsVector_.emplace_back(std::thread{&PathFinder::findBestPathThreadFunction, this, std::ref(paths[i])});
    }
    for (auto& th : threadsVector_) {
        th.join();
    }

    size_t bestLengthIndex = 0;
    size_t bestLength = this->getPathLength(paths.front());
    size_t tmpLength;
    for (size_t i = 1; i < paths.size(); ++i) {
        tmpLength = this->getPathLength(paths[i]);
        if (tmpLength < bestLength) {
            bestLength = tmpLength;
            bestLengthIndex = i;
        }
    }

    return paths[bestLengthIndex];
}

#include "pathFinder.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

PathFinder::PathFinder() : tabuList_(tabuListSize_, {emptyTabuFieldValue_, emptyTabuFieldValue_}) {}

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

bool PathFinder::isOnTabuList(const std::pair<int, int>& node) {
    return std::any_of(tabuList_.begin(), tabuList_.end(), [&](auto& el){
        return el == node;
    });
}

std::pair<int, int> PathFinder::getBestNeighbour(std::vector<size_t>& path) {
    std::pair<int, int> bestNeighbour{emptyTabuFieldValue_, emptyTabuFieldValue_};
    size_t bestNeighbourLength = std::numeric_limits<size_t>::max();
    size_t tmp_length = 0;
    for (size_t first = 1; first < path.size() - 1; ++first) {
        for (size_t second = first + 1; second < path.size(); ++second) {
            if (this->isOnTabuList({first, second})) {
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

void PathFinder::updateTabuList(const std::pair<int, int>& value) {
    tabuList_[tabuCurrentIndex_] = value;
    ++tabuCurrentIndex_;
    if (tabuCurrentIndex_ >= tabuList_.size()) {
        tabuCurrentIndex_ = 0;
    }
}

void PathFinder::cleanTabuList() {
    for (auto& [first, second] : tabuList_) {
        first = emptyTabuFieldValue_;
        second = emptyTabuFieldValue_;
    }
    tabuCurrentIndex_ = 0;
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

bool PathFinder::isNodeInPath(const std::vector<size_t>& path, size_t node) {
    return std::any_of(path.begin(), path.end(), [&](auto value){
        return value == node;
    });
}

std::vector<size_t> PathFinder::findBestPath() {
    std::vector<size_t> path(distances_.size(), 0);
    this->generateInitialPath(path);
    auto bestPath = path;
    size_t pathLength = this->getPathLength(path);
    size_t bestPathLength = pathLength;

    std::pair<int, int> bestNeighbour;
    for (size_t i = 0; i < iterationsCount_; ++i) {
        bestNeighbour = this->getBestNeighbour(path);
        if (bestNeighbour.first != -1 && bestNeighbour.second != -1) {
            std::swap(path[bestNeighbour.first], path[bestNeighbour.second]);
            this->updateTabuList(bestNeighbour);
            pathLength = this->getPathLength(path);
            if (pathLength < bestPathLength) {
                bestPathLength = pathLength;
                bestPath = path;
            }
        } else {
            this->cleanTabuList();
        }
    }
    return bestPath;
}

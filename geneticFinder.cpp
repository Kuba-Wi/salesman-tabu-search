#include "geneticFinder.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

void GeneticFinder::readDistancesFromFile(const std::string& filename) {
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

std::vector<size_t> GeneticFinder::findBestPath() {
    constexpr size_t ITERATION_COUNT = 100;
    constexpr size_t THREADS_COUNT = 5;
    constexpr size_t ELITE_SIZE = 1;

    this->generateInitialPopulation();
    std::vector<size_t> bestPath = population_.front();
    std::vector<std::vector<size_t>> newPopulation(population_.size());
    std::srand(std::time(nullptr));

    for (size_t iter = 0; iter < ITERATION_COUNT; ++iter) {
        for (size_t i = 0; i < ELITE_SIZE; ++i) {
            newPopulation[i] = population_[i];
        }

        for (size_t i = 0; i < THREADS_COUNT; ++i) {
            threadsVector_.emplace_back(&GeneticFinder::crossPathsThreadFun, 
                                        this, 
                                        std::ref(newPopulation), 
                                        (population_.size() / THREADS_COUNT) * i + (i == 0 ? ELITE_SIZE : 0),
                                        (population_.size() / THREADS_COUNT) * (i + 1));
        }
        
        for (auto& th : threadsVector_) {
            th.join();
        }
        threadsVector_.clear();

        for (size_t i = 0; i < newPopulation.size() / 2; ++i) {
            this->mutatePath(newPopulation[std::rand() % (population_.size())]);
        }
        population_ = newPopulation;
        this->orderPopulation();
        if (this->getPathLength(population_.front()) < this->getPathLength(bestPath)) {
            bestPath = population_.front();
        }
    }

    return bestPath;
}

size_t GeneticFinder::getPathLength(const std::vector<size_t>& path) const {
    size_t length = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        length += distances_[path[i]][path[i + 1]];
    }
    length += distances_[path.front()][path.back()];
    return length;
}

void GeneticFinder::generateInitialPopulation() {
    constexpr size_t POPULATION_SIZE = 200;
    population_ = std::vector<std::vector<size_t>>(POPULATION_SIZE, std::vector<size_t>(distances_.size(), 0));
    for (auto& path : population_) {
        this->generateInitialPath(path);
    }
    this->orderPopulation();
}

void GeneticFinder::generateInitialPath(std::vector<size_t>& path) const {
    size_t nodeIndex = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        while (this->isNodeInPath(path, nodeIndex)) {
            nodeIndex = std::rand() % path.size();
        }
        path[i] = nodeIndex;
    }
}

bool GeneticFinder::isNodeInPath(const std::vector<size_t>& path, size_t node) const {
    return std::any_of(path.begin(), path.end(), [&](auto value){
        return value == node;
    });
}

std::vector<size_t> GeneticFinder::crossPaths(const std::vector<size_t>& first, const std::vector<size_t>& second) const {
    size_t begin = 0, end = 0;
    do {
        std::lock_guard lg{randMx_};
        begin = std::rand() % first.size();
        end = std::rand() % first.size();
    } while (begin == end || ((begin == 0) & (end == (first.size() - 1))));

    if (begin > end) {
        std::swap(begin, end);
    }

    auto crossedPath = first;
    auto secondCp = second;
    for (size_t i = begin; i <= end; ++i) {
        secondCp.erase(std::remove(secondCp.begin(), secondCp.end(), first[i]), secondCp.end());
    }

    for (size_t i = 0; i < begin; ++i) {
        crossedPath[i] = secondCp[i];
    }
    for (size_t j = end + 1; j < crossedPath.size(); ++j) {
        crossedPath[j] = secondCp[j - (end - begin + 1)];
    }

    return crossedPath;
}

void GeneticFinder::mutatePath(std::vector<size_t>& path) const {
    size_t first = (std::rand() % path.size());
    first = (first == 0) ? 1 : first; 
    size_t second = std::rand() % path.size();
    second = (second == 0) ? 1 : second; 
    std::swap(path[first], path[second]);
}

void GeneticFinder::orderPopulation() {
    std::sort(population_.begin(), population_.end(), [this](auto& first, auto& second){
        return this->getPathLength(first) < this->getPathLength(second);
    });
}

void GeneticFinder::crossPathsThreadFun(std::vector<std::vector<size_t>>& newPopulation, size_t begin, size_t end) const {
    size_t firstParent, secondParent;
    for (size_t i = begin; i < end; ++i) {
        do {
            std::lock_guard lg{randMx_};
            firstParent = std::rand() % (population_.size() / 2);
            secondParent = std::rand() % (population_.size() / 2);
        } while (firstParent == secondParent);

        newPopulation[i] = this->crossPaths(population_[firstParent], population_[secondParent]);
    }
}
#include "geneticFinder.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
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
    this->generateInitialPopulation();
    std::vector<size_t> bestPath = population_.front();
    std::vector<std::vector<size_t>> newPopulation(population_.size());
    std::srand(std::time(nullptr));

    for (size_t iter = 0; iter < ITERATION_COUNT_; ++iter) {
        for (size_t i = 0; i < ELITE_SIZE_; ++i) {
            newPopulation[i] = population_[i];
        }

        for (size_t i = 0; i < THREADS_COUNT_; ++i) {
            threadsVector_.emplace_back(&GeneticFinder::crossPathsThreadFun, 
                                        this, 
                                        std::ref(newPopulation), 
                                        (population_.size() / THREADS_COUNT_) * i + (i == 0 ? ELITE_SIZE_ : 0),
                                        (population_.size() / THREADS_COUNT_) * (i + 1));
        }
        
        for (auto& th : threadsVector_) {
            th.join();
        }
        threadsVector_.clear();

        constexpr size_t mutationRate = 100;
        for (size_t i = 0; i < newPopulation.size() / mutationRate; ++i) {
            this->mutatePath(newPopulation[std::rand() % (population_.size())]);
        }
        population_ = newPopulation;

        auto shortestIt = std::min_element(population_.begin(), population_.end(), [this](auto& first, auto& second){
            return this->getPathLength(first) < this->getPathLength(second);
        });
        if (this->getPathLength(*shortestIt) < this->getPathLength(bestPath)) {
            bestPath = *shortestIt;
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
    population_ = std::vector<std::vector<size_t>>(POPULATION_SIZE_, std::vector<size_t>(distances_.size(), 0));
    for (auto& path : population_) {
        this->generateInitialPath(path);
    }
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
    size_t tmpParent;
    constexpr size_t PARENTS_SIZE = 10;
    std::vector<size_t> parents(PARENTS_SIZE, std::numeric_limits<size_t>::max());
    for (size_t i = begin; i < end; ++i) {
        for (size_t index = 0; index < PARENTS_SIZE; ++index) {
            do {
            std::lock_guard lg{randMx_};
            tmpParent = std::rand() % (population_.size());
            } while (std::any_of(parents.begin(), parents.end(), [=](auto n){ return n == tmpParent; }));
            parents[index] = tmpParent;
        }

        std::lock_guard lg{populationMx_};
        std::sort(parents.begin(), parents.end(), [this](auto& first, auto& second){
            return this->getPathLength(population_[first]) < this->getPathLength(population_[second]);
        });
        newPopulation[i] = this->crossPaths(population_[parents[0]], population_[parents[1]]);
    }

    // Roulette crossing
    // std::unique_lock ul{populationMx_};
    // std::vector<double> probabilities(population_.size());
    // auto shortestIt = std::min_element(population_.begin(), population_.end(), [this](auto& first, auto& second){
    //     return this->getPathLength(first) < this->getPathLength(second);
    // });
    // auto longestIt = std::max_element(population_.begin(), population_.end(), [this](auto& first, auto& second){
    //     return this->getPathLength(first) < this->getPathLength(second);
    // });
    // const double diff = this->getPathLength(*longestIt) - this->getPathLength(*shortestIt);
    // for (size_t i = 0; i < population_.size(); ++i) {
    //     probabilities[i] = (this->getPathLength(*longestIt) - this->getPathLength(population_[i])) / diff;
    // }
    // ul.unlock();
    // std::discrete_distribution<unsigned int> distribution(probabilities.begin(), probabilities.end());
    // std::mt19937 generator(std::random_device{}());

    // for (size_t i = begin; i < end; ++i) {
    //     std::lock_guard lg(populationMx_);
    //     randMx_.lock();
    //     auto first = population_[distribution(generator)];
    //     auto second = population_[distribution(generator)];
    //     randMx_.unlock();
    //     newPopulation[i] = this->crossPaths(first, second);
    // }
}

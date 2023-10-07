#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

size_t get_path_length(const std::vector<std::vector<size_t>>& distances, const std::vector<size_t>& path) {
    size_t length = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        length += distances[path[i]][path[i + 1]];
    }
    length += distances[path.front()][path.back()];
    return length;
}

bool is_on_tabu_list(size_t first_index, size_t second_index, const std::vector<std::pair<int, int>>& tabu_list) {
    return std::any_of(tabu_list.begin(), tabu_list.end(), [&](auto& el){
        return el.first == int(first_index) && el.second == int(second_index);
    });
}

std::pair<int, int> get_best_neighbour(const std::vector<std::vector<size_t>>& distances, 
                                       std::vector<size_t>& path, 
                                       const std::vector<std::pair<int, int>>& tabu_list) {

    std::pair<int, int> best_neighbour{-1, -1};
    size_t best_neighbour_length = std::numeric_limits<size_t>::max();
    size_t tmp_length = 0;
    for (size_t first = 1; first < path.size() - 1; ++first) {
        for (size_t second = first + 1; second < path.size(); ++second) {
            if (is_on_tabu_list(first, second, tabu_list)) {
                continue;
            }
            std::swap(path[first], path[second]);
            tmp_length = get_path_length(distances, path);
            if (tmp_length < best_neighbour_length) {
                best_neighbour_length = tmp_length;
                best_neighbour.first = first;
                best_neighbour.second = second;
            }
            std::swap(path[first], path[second]);
        }
    }
    return best_neighbour;
}

void update_tabu_list(std::vector<std::pair<int, int>>& tabu_list, size_t& tabu_current_index, const std::pair<int, int>& value) {
    tabu_list[tabu_current_index] = value;
    ++tabu_current_index;
    if (tabu_current_index >= tabu_list.size()) {
        tabu_current_index = 0;
    }
}

void clean_tabu_list(std::vector<std::pair<int, int>>& tabu_list, size_t& tabu_current_index) {
    for (auto& [first, second] : tabu_list) {
        first = -1;
        second = -1;
    }
    tabu_current_index = 0;
}

std::vector<size_t> find_best_path(const std::vector<std::vector<size_t>>& distances) {
    constexpr size_t TABU_LIST_SIZE = 7;
    constexpr size_t ITERATIONS = 100;
    std::vector<size_t> path(distances.size());
    std::iota(path.begin(), path.end(), 0);
    auto best_path = path;
    size_t path_length = get_path_length(distances, path);
    size_t best_path_length = path_length;
    std::vector<std::pair<int, int>> tabu_list(TABU_LIST_SIZE, {-1, -1});
    size_t tabu_current_index = 0;

    std::pair<int, int> best_neighbour;
    for (size_t i = 0; i < ITERATIONS; ++i) {
        best_neighbour = get_best_neighbour(distances, path, tabu_list);
        if (best_neighbour.first != -1 && best_neighbour.second != -1) {
            std::swap(path[best_neighbour.first], path[best_neighbour.second]);
            update_tabu_list(tabu_list, tabu_current_index, best_neighbour);
            path_length = get_path_length(distances, path);
            if (path_length < best_path_length) {
                best_path_length = path_length;
                best_path = path;
            }
        } else {
            clean_tabu_list(tabu_list, tabu_current_index);
        }
    }
    return best_path;
}

int main() {
    std::vector<std::vector<size_t>> distances = {{0, 10, 15, 20},
                                                  {10, 0, 35, 25},
                                                  {15, 35, 0, 30},
                                                  {20, 25, 30, 0}};

    auto best = find_best_path(distances);
    auto length = get_path_length(distances, best);
    for (auto node : best) {
        std::cout << node << " ";
    }
    std::cout << "\n";
    std::cout << length << "\n";
}

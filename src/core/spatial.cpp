#include <cmath>
#include <stdexcept>
#include "pancake/core/spatial.hpp"

namespace Pancake {

    template<class T>
    SpatialHashGrid<T>::SpatialHashGrid(float gridSize) {
        this->gridSize = std::abs(gridSize);
    }

    template<class T>
    int SpatialHashGrid<T>::getGridSize() {
        return this->gridSize;
    }

    template<class T>
    void SpatialHashGrid<T>::add(T element, float x, float y, float w, float h) {

        if (this->registrations.contains(element)) {this->remove(element);}
        std::vector<std::tuple<int, int>> coordinates;
        std::tuple<int, int> coordinate;

        w = std::abs(w);
        h = std::abs(h);
        int x_min = std::floor((x - 0.5f * w) / this->gridSize);
        int y_min = std::floor((y - 0.5f * h) / this->gridSize);
        int x_max = std::floor((x + 0.5f * w) / this->gridSize);
        int y_max = std::floor((y + 0.5f * h) / this->gridSize);
        
        for (int i = x_min; i <= x_max; i++) {
            for (int j = y_min; j <= y_max; j++) {
                coordinate = {i, j};
                coordinates.push_back(coordinate);
                if (!this->grid.contains(coordinate)) {std::unordered_set<T> set; this->grid.insert({coordinate, set});}
                auto search = this->grid.find(coordinate);
                search->second.insert(element);
            }
        }

        this->registrations.insert({element, coordinates});

    }

    template<class T>
    void SpatialHashGrid<T>::add(T element, glm::vec2 min, glm::vec2 max) {
        float x = 0.5f * (min.x + max.x);
        float y = 0.5f * (min.y + max.y);
        float w = max.x - min.x;
        float h = max.y - min.y;
        this->add(element, x, y, w, h);
    }

    template<class T>
    void SpatialHashGrid<T>::update(T element, float x, float y, float w, float h) {
        this->add(element, x, y, w, h);
    }

    template<class T>
    void SpatialHashGrid<T>::update(T element, glm::vec2 min, glm::vec2 max) {
        this->add(element, min, max);
    }

    template<class T>
    std::unordered_set<T> SpatialHashGrid<T>::get(float x, float y, float w, float h) {
        
        std::unordered_set<T> result;
        std::tuple<int, int> coordinate;

        w = std::abs(w);
        h = std::abs(h);
        int x_min = std::floor((x - 0.5f * w) / this->gridSize);
        int y_min = std::floor((y - 0.5f * h) / this->gridSize);
        int x_max = std::floor((x + 0.5f * w) / this->gridSize);
        int y_max = std::floor((y + 0.5f * h) / this->gridSize);
        
        for (int i = x_min; i <= x_max; i++) {
            for (int j = y_min; j <= y_max; j++) {
                coordinate = {i, j};
                if (!this->grid.contains(coordinate)) {continue;}
                auto search = this->grid.find(coordinate);
                for (const auto& element : search->second) {result.insert(element);}
            }
        }
        
        return result;

    }

    template<class T>
    std::unordered_set<T> SpatialHashGrid<T>::get(int x, int y) {

        std::unordered_set<T> result;
        std::tuple<int, int> coordinate = {x, y};

        if (!this->grid.contains(coordinate)) {return result;}
        auto search = this->grid.find(coordinate);
        return search->second;

    }

    template<class T>
    void SpatialHashGrid<T>::remove(T element) {

        auto s0 = this->registrations.find(element);
        if (s0 == this->registrations.end()) {return;}
        
        for (std::tuple<int, int> location : s0->second) {
            auto s1 = this->grid.find(location);
            if (s1 == this->grid.end()) {continue;}
            s1->second.erase(element);
            if (s1->second.empty()) {this->grid.erase(location);}
        }

        this->registrations.erase(element);
    
    }

    template<class T>
    void SpatialHashGrid<T>::clear() {
        this->grid.clear();
        this->registrations.clear();
    }

    template<class T>
    auto SpatialHashGrid<T>::begin() {
        return this->grid.begin();
    }

    template<class T>
    auto SpatialHashGrid<T>::end() {
        return this->grid.end();
    }

}


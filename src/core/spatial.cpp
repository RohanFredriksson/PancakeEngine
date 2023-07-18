#include <cmath>
#include <stdexcept>
#include "pancake/core/spatial.hpp"

namespace Pancake {

    template<class T>
    SpatialHashGrid<T>::SpatialHashGrid(float grid_size) {
        this->grid_size = grid_size;
    }

    template<class T>
    void SpatialHashGrid<T>::add(T element, float x, float y, float w, float h) {

        if (this->registrations.contains(element)) {this->remove(element);}
        std::vector<std::tuple<int, int>> coordinates;
        std::tuple<int, int> coordinate;

        int x_min = std::floor((x - 0.5f * w) / this->grid_size);
        int y_min = std::floor((y - 0.5f * h) / this->grid_size);
        int x_max = std::floor((x + 0.5f * w) / this->grid_size);
        int y_max = std::floor((y + 0.5f * h) / this->grid_size);
        
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
    void SpatialHashGrid<T>::update(T element, float x, float y, float w, float h) {
        this->add(element, x, y, w, h);
    }

    template<class T>
    void SpatialHashGrid<T>::remove(T element) {

        auto s0 = this->registrations.find(element);
        if (s0 == this->registrations.end()) {return;}
        
        for (std::tuple<int, int> location : s0->second) {
            auto s1 = this->grid.find(location);
            if (s1 == this->grid.end()) {continue;}
            s1->second.erase(element);
        }

        this->registrations.erase(element);
    
    }

    template<class T>
    void SpatialHashGrid<T>::clear() {
        this->grid.clear();
        this->registrations.clear();
    }

    template<class T>
    std::unordered_set<T> SpatialHashGrid<T>::get(float x, float y) {
        std::unordered_set<T> result;
        return result;
    }

}


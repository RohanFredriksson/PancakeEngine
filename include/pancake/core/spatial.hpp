#pragma once

#include <tuple>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <glm/glm.hpp>

namespace Pancake {

    struct IntTupleHash {
        size_t operator()(const std::tuple<int, int>& t) const {
            const std::hash<int> hasher;
            std::size_t seed = 0;
            seed ^= hasher(std::get<0>(t)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hasher(std::get<1>(t)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

    struct IntTupleEqual {
        bool operator()(const std::tuple<int, int>& lhs, const std::tuple<int, int>& rhs) const {
            return (std::get<0>(lhs) == std::get<0>(rhs)) && (std::get<1>(lhs) == std::get<1>(rhs));
        }
    };

    template<class T>
    class SpatialHashGrid {

        private: 

            int gridSize;
            std::unordered_map< std::tuple<int, int>, std::unordered_set<T>, IntTupleHash, IntTupleEqual > grid;
            std::unordered_map< T, std::vector<std::tuple<int, int>> > registrations;

        public:

            SpatialHashGrid<T>(float gridSize) {
                this->gridSize = std::abs(gridSize);
            }

            int getGridSize() {
                return this->gridSize;
            }

            void add(T element, float x, float y, float w, float h) {

                auto it = this->registrations.find(element);
                if (it != this->registrations.end()) {this->remove(element);}

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
                        
                        auto it = this->grid.find(coordinate);
                        if (it == this->grid.end()) {std::unordered_set<T> set; this->grid.insert({coordinate, set});}

                        auto search = this->grid.find(coordinate);
                        search->second.insert(element);

                    }
                }

                this->registrations.insert({element, coordinates});

            }

            void add(T element, glm::vec2 min, glm::vec2 max) {
                float x = 0.5f * (min.x + max.x);
                float y = 0.5f * (min.y + max.y);
                float w = max.x - min.x;
                float h = max.y - min.y;
                this->add(element, x, y, w, h);
            }

            void update(T element, float x, float y, float w, float h) {
                this->add(element, x, y, w, h);
            }

            void update(T element, glm::vec2 min, glm::vec2 max) {
                this->add(element, min, max);
            }

            std::unordered_set<T> get(float x, float y, float w, float h) {
                
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
                        auto search = this->grid.find(coordinate);
                        if (search == this->grid.end()) {continue;}
                        for (const auto& element : search->second) {result.insert(element);}
                    }
                }
                
                return result;

            }

            std::unordered_set<T> get(int x, int y) {

                std::unordered_set<T> result;
                std::tuple<int, int> coordinate = {x, y};

                auto search = this->grid.find(coordinate);
                if (search == this->grid.end()) {return result;}
                return search->second;

            }

            void remove(T element) {

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

            void clear() {
                this->grid.clear();
                this->registrations.clear();
            }

            auto begin() {
                return this->grid.begin();
            }

            auto end() {
                return this->grid.end();
            }

    };

}
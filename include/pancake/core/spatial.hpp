#pragma once

#include <tuple>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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

            SpatialHashGrid<T>(float gridSize);            
            int getGridSize();
            void add(T element, float x, float y, float w, float h);
            void update(T element, float x, float y, float w, float h);
            std::unordered_set<T> get(float x, float y, float w, float h);
            std::unordered_set<T> get(int x, int y);
            void remove(T element);
            void clear();
            auto begin();
            auto end();

    };

}
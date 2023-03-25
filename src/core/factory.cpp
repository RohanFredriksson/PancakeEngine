#include <unordered_map>
#include "pancake/core/factory.hpp"

using std::unordered_map;

namespace {
    unordered_map<string, Component*(*)()> components;
}

namespace Factory {

    void add(string type, Component*(*method)()) {
        components[type] = method;
    }

    Component* create(string type) {
        auto search = components.find(type);
        if (search == components.end()) {return NULL;}
        return search->second(); 
    }

};
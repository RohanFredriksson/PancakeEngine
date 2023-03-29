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

    Component* load(json j) {
        
        if (!j.contains("type") || !j["type"].is_string()) {return NULL;}
        auto search = components.find(j["type"]);
        if (search == components.end()) {return NULL;}
        
        Component* component = search->second();
        if (!component->load(j)) {
            delete component; 
            return NULL;
        }

        return component;
    }

};
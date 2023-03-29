#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "pancake/core/component.hpp"

using std::string;
using json = nlohmann::json;

namespace Factory {
    void add(string type, Component* (*create)());
    Component* create(string type);
    Component* load(json);
}
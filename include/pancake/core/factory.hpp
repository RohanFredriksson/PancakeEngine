#pragma once

#include <string>
#include "pancake/core/component.hpp"

using std::string;

namespace Factory {
    void add(string type, Component* (*create)());
    Component* create(string type);
}
#pragma once

#include "pancake/core/scene.hpp"

namespace Engine {
    int run(string name, string filename, void(*init)(Scene scene));
    int run(string name, void (*init)(Scene* scene));
    int run(string name, string filename);
}
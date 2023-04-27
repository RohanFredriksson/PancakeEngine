#pragma once

#include <string>
#include <nlohmann/json.hpp>
using std::string;
using json = nlohmann::json;

namespace Pancake {

    namespace Spritesheet {
        void load(string filename);
        void clear();
        json serialise();
    }

}
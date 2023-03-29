#pragma once

#include <cstring>
#include <nlohmann/json.hpp>

#include "pancake/graphics/primitives/texture.hpp"
#include "pancake/graphics/primitives/sprite.hpp"
#include "pancake/graphics/primitives/font.hpp"
#include "pancake/audio/audiowave.hpp"

using std::string;
using json = nlohmann::json;

namespace AssetPool {

    void init();
    void clear();
    void destroy();

}

namespace TexturePool {

    void init();
    void clear();
    void destroy();
    Texture* get(string name);

}

namespace SpritePool {

    void init();
    void clear();
    void destroy();
    json serialise();
    Sprite* get(string name);
    bool has(string name);
    void put(Sprite* sprite);

}

namespace FontPool {

    void init();
    void clear();
    void destroy();
    json serialise();
    Font* get(string name);
    Font* get(string name, float size);

}

namespace AudioPool {

    void init();
    void clear();
    void destroy();
    AudioWave* get(string name);

}
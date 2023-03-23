#pragma once

#include <cstring>
#include "pancake/graphics/primitives/texture.hpp"
#include "pancake/graphics/primitives/sprite.hpp"
#include "pancake/graphics/primitives/font.hpp"
#include "pancake/audio/audiowave.hpp"

using std::string;

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
    Sprite* get(string name);
    bool has(string name);
    void put(Sprite* sprite);

}

namespace FontPool {

    void init();
    void clear();
    void destroy();
    Font* get(string name);

}

namespace AudioPool {

    void init();
    void clear();
    void destroy();
    AudioWave* get(string name);

}
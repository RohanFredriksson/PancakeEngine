#pragma once

#include <string.h>
#include "graphics/primitives/texture.hpp"
#include "graphics/primitives/sprite.hpp"
#include "graphics/primitives/font.hpp"
#include "audio/audiowave.hpp"

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

}

namespace FontPool {

    void init();
    void clear();
    void destroy();
    Font* get(string name, float size);

}

namespace AudioPool {

    void init();
    void clear();
    void destroy();
    AudioWave* get(string name);

}
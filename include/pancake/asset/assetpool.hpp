#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "pancake/graphics/texture.hpp"
#include "pancake/graphics/sprite.hpp"
#include "pancake/graphics/font.hpp"
#include "pancake/audio/audiowave.hpp"

namespace Pancake {

    namespace AssetPool {

        void init();
        void clear();
        void destroy();

    }

    namespace TexturePool {

        void init();
        void clear();
        void destroy();
        Texture* get(std::string name);

    }

    namespace SpritePool {

        void init();
        void clear();
        void destroy();
        nlohmann::json serialise();
        Sprite* get(std::string name);
        bool has(std::string name);
        void put(Sprite* sprite);

    }

    namespace FontPool {

        void init();
        void clear();
        void destroy();
        nlohmann::json serialise();
        Font* get(std::string name);
        Font* get(std::string name, float size);
        bool has(std::string name);
        bool has(std::string name, float size);

    }

    namespace AudioPool {

        void init();
        void clear();
        void destroy();
        nlohmann::json serialise();
        AudioWave* get(std::string name);

    }

}
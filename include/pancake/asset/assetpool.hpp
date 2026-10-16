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

        // Hot reload: textures and audio loaded from files are read again when the files change.
        // Prefabs are always read again when they change. Sprites and players keep using the same objects.
        int reloadChanged();            // Reload now. Returns how many assets were reloaded.
        void setHotReload(bool enabled);
        bool hasHotReload();
        void update(float dt);          // Checks twice a second while hot reload is on. Called by the main loop.

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
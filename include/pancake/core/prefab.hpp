#pragma once

#include <string>
#include <glm/glm.hpp>

#include "pancake/core/entity.hpp"

namespace Pancake {

    // Prefabs are entities saved to json files and copied into scenes with new ids. Files are cached, and
    // read again when they change. Assets the entity uses, such as sprites, must already be loaded.
    namespace Prefab {

        bool save(Entity* entity, std::string filename);
        Entity* load(std::string filename);                           // A new copy, not yet in a scene, or nullptr.
        Entity* spawn(std::string filename, glm::vec2 position);      // A new copy added to the current scene.
        void clearCache();

    }

}

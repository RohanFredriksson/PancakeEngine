#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>

#include "pancake/core/prefab.hpp"
#include "pancake/core/window.hpp"

namespace Pancake {

    namespace {

        struct CachedPrefab {
            std::filesystem::file_time_type modified;
            nlohmann::json json;
        };

        std::unordered_map<std::string, CachedPrefab> cache;

        // The prefab's json, read again if the file has changed since it was cached.
        bool read(std::string filename, nlohmann::json* out) {

            std::error_code error;
            std::filesystem::file_time_type modified = std::filesystem::last_write_time(filename, error);
            if (error) {
                std::cout << "ERROR::PREFAB::FILE_NOT_FOUND: '" << filename << "'\n";
                return false;
            }

            auto it = cache.find(filename);
            if (it != cache.end() && it->second.modified == modified) {*out = it->second.json; return true;}

            std::ifstream file(filename);
            nlohmann::json j = nlohmann::json::parse(file, nullptr, false);
            if (j.is_discarded() || !j.is_object()) {
                std::cout << "ERROR::PREFAB::INVALID_JSON: '" << filename << "'\n";
                return false;
            }

            cache[filename] = {modified, j};
            *out = j;
            return true;

        }

    }

    namespace Prefab {

        bool save(Entity* entity, std::string filename) {
            if (entity == nullptr) {return false;}
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cout << "ERROR::PREFAB::SAVE::FILE_NOT_OPENED: '" << filename << "'\n";
                return false;
            }
            file << entity->serialise().dump(4);
            return file.good();
        }

        Entity* load(std::string filename) {
            nlohmann::json j;
            if (!read(filename, &j)) {return nullptr;}
            Entity* e = Entity::instantiate(j);
            if (e == nullptr) {std::cout << "ERROR::PREFAB::INVALID_ENTITY: '" << filename << "'\n";}
            return e;
        }

        Entity* spawn(std::string filename, glm::vec2 position) {
            Scene* scene = Window::getScene();
            if (scene == nullptr) {return nullptr;}
            Entity* e = Prefab::load(filename);
            if (e == nullptr) {return nullptr;}
            e->setPosition(position);
            scene->addEntity(e);
            return e;
        }

        void clearCache() {
            cache.clear();
        }

    }

}

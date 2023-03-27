#include <vector>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "pancake/util/spritesheet.hpp"
#include "pancake/util/assetpool.hpp"

using std::vector;
using glm::vec2;
using json = nlohmann::json;

namespace {

    char jsonext[] = ".json";

    class Metadata {
        
        public:
            
            bool valid;
            int width;
            int height;
            vector<string> names;

            Metadata(bool valid, int width, int height) {
                this->valid = valid;
                this->width = width;
                this->height = height;
            }

    };

    string getMetadataName(string filename) {

        int i = 0;
        int lastDot = -1;
        const char* f = filename.c_str();

        while (f[i] != '\0') {
            if (f[i] == '.') {lastDot = i;}
            i++;
        }

        char* metaname = (char*) malloc((lastDot+strlen(jsonext) + 1));
        memcpy(metaname, f, lastDot);
        memcpy(metaname + lastDot, jsonext, strlen(jsonext) + 1);
        string result = metaname;
        free(metaname);

        return result;

    }

    Metadata getMetadata(string metaname) {

        Metadata result(false, 0, 0);

        std::ifstream f(metaname);
        json metadata = json::parse(f);

        if (!metadata.contains("spriteWidth") || !metadata["spriteWidth"].is_number_integer()) {
            std::cout << "ERROR::SPRITESHEET::LOAD::JSON_PARSE_ERROR: 'spriteWidth' could not be parsed.\n";
            return result;
        }  

        if (!metadata.contains("spriteHeight") || !metadata["spriteHeight"].is_number_integer()) {
            std::cout << "ERROR::SPRITESHEET::LOAD::JSON_PARSE_ERROR: 'spriteHeight' could not be parsed.\n";
            return result;
        }

        int width = metadata["spriteWidth"];
        int height = metadata["spriteHeight"];

        if (width <= 0 || height <= 0) {
            std::cout << "ERROR::SPRITESHEET::LOAD::JSON_PARSE_ERROR: invalid dimensions.\n";
            return result;
        }

        if (!metadata.contains("sprites") || !metadata["sprites"].is_array()) {
            std::cout << "ERROR::SPRITESHEET::LOAD::JSON_PARSE_ERROR: 'sprites' could not be parsed.\n";
            return result;
        }  

        vector<string> names;
        for (auto& element : metadata["sprites"]) {
            if (!element.is_string()) {
                std::cout << "ERROR::SPRITESHEET::LOAD::JSON_PARSE_ERROR: 'sprites' could not be parsed.\n";
                return result;
            }
            names.push_back(element);
        }

        result.valid = true;
        result.width = width;
        result.height = height;
        result.names = names;
        return result;

    }

}

namespace Spritesheet {

    void load(string filename) {

        string metaname = getMetadataName(filename);
        Metadata metadata = getMetadata(metaname);

        if (!metadata.valid) {
            return;
        }

        Texture* texture = TexturePool::get(filename);
        int currentX = 0;
        int currentY = texture->getHeight() - metadata.height;

        for (int i = 0; i < metadata.names.size(); i++) {

            if (currentY < 0) {
                std::cout << "ERROR::SPRITESHEED::LOAD::SPRITE_LOAD: '" << metadata.names[i] << "' could not be loaded.\n";
                continue;
            }

            float topY = (currentY + metadata.height) / (float) texture->getHeight();
            float rightX = (currentX + metadata.width) / (float) texture->getWidth();
            float leftX = currentX / (float) texture->getWidth();
            float bottomY = currentY / (float) texture->getHeight();

            vec2 texCoords[4];
            texCoords[0].x = rightX;
            texCoords[0].y = topY;
            texCoords[1].x = rightX;
            texCoords[1].y = bottomY;
            texCoords[2].x = leftX;
            texCoords[2].y = bottomY;
            texCoords[3].x = leftX;
            texCoords[3].y = topY;

            // If the sprite name is already taken, do not overwrite it.
            if (!SpritePool::has(metadata.names[i])) {
                Sprite* sprite = new Sprite(metadata.names[i], texture, texCoords);
                SpritePool::put(sprite);
            }

            currentX += metadata.width;
            if (currentX >= texture->getWidth()) {
                currentX = 0;
                currentY -= metadata.height;
            }

        }

    }

}
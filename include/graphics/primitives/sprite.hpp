#pragma once

#include <array>
#include <string>
#include <glm/glm.hpp>
#include "graphics/primitives/texture.hpp"

using std::array;
using std::string;
using glm::vec2;

class Sprite {

    private:

        string name;
        Texture* texture;
        vec2 texCoords[4];
        
        void init(string name, Texture* texture, vec2 texCoords[4]);

    public:

        Sprite();
        Sprite(string name, Texture* texture);
        Sprite(string name, Texture* texture, vec2 texCoords[4]);

        string getName();
        Texture* getTexture();
        vec2* getTexCoords();

        void setName(string name);
        void setTexture(Texture* texture);
        void setTexCoords(vec2 texCoords[4]);

};
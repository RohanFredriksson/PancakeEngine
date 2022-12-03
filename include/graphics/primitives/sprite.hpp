#pragma once

#include <array>
#include <string>
#include <glm/glm.hpp>

using std::array;
using std::string;
using glm::vec2;

class Sprite {

    private:

        string name;
        Texture* texture;
        vec2 texCoords[4];

    public:

        Sprite(string name, Texture* texture, vec2 texCoords[4]);
        ~Sprite();

        string getName();
        Texture* getTexture();
        array<vec2, 4> getTexCoords();

};
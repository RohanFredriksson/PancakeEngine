#include "graphics/primitives/sprite.hpp"

Sprite::Sprite(string name, Texture* texture, vec2 texCoords[4]) {
    
    this->name = name;
    this->texture = texture;
    
    this->texCoords[0] = texCoords[0];
    this->texCoords[1] = texCoords[1];
    this->texCoords[2] = texCoords[2];
    this->texCoords[3] = texCoords[3];

}

string Sprite::getName() {
    return this->name;
}

Texture* Sprite::getTexture() {
    return this->texture;
}

vec2* Sprite::getTexCoords() {
    return this->texCoords;
}
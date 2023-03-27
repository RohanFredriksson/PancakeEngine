#include "pancake/graphics/primitives/sprite.hpp"

void Sprite::init(string name, Texture* texture, vec2 texCoords[4]) {
    
    this->name = name;
    this->texture = texture;
    
    this->texCoords[0] = texCoords[0];
    this->texCoords[1] = texCoords[1];
    this->texCoords[2] = texCoords[2];
    this->texCoords[3] = texCoords[3];

}

Sprite::Sprite() {
    
}

Sprite::Sprite(string name, Texture* texture) {

    vec2 texCoords[4];
    texCoords[0] = vec2(1, 1);
    texCoords[1] = vec2(1, 0);
    texCoords[2] = vec2(0, 0);
    texCoords[3] = vec2(0, 1);

    this->init(name, texture, texCoords);
    
}

Sprite::Sprite(string name, Texture* texture, vec2 texCoords[4]) {
    this->init(name, texture, texCoords);
}

json Sprite::serialise() {
    json j;
    j.emplace("name", this->name);
    j.emplace("texture", this->texture->getName());
    j.emplace("texCoords", json::array());
    for (int i = 0; i < 4; i++) {
        json v = json::array();
        v.push_back(this->texCoords[i].x);
        v.push_back(this->texCoords[i].y);
        j.push_back(v);
    }
    return j;
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

void Sprite::setName(string name) {
    this->name = name;
}

void Sprite::setTexture(Texture* texture) {
    this->texture = texture;
}

void Sprite::setTexCoords(vec2 texCoords[4]) {
    this->texCoords[0] = texCoords[0];
    this->texCoords[1] = texCoords[1];
    this->texCoords[2] = texCoords[2];
    this->texCoords[3] = texCoords[3];
}
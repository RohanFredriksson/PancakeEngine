#include "graphics/renderer/spriterenderer.hpp"

SpriteRenderer::SpriteRenderer(Sprite* sprite, vec4 colour, int zIndex) : Component() {
    
    this->sprite = sprite;
    this->colour = colour;
    this->zIndex = zIndex;

    this->lastSprite = sprite;
    this->lastColour = colour;
    this->lastZIndex = zIndex;

    this->dirty = true;

}

void SpriteRenderer::update(float dt) {

    if (this->sprite != this->lastSprite) {
        this->lastSprite = this->sprite;
        this->dirty = true;
    }

    if (this->colour != this->lastColour) {
        this->lastColour = this->colour;
        this->dirty = true;
    }

    if (this->zIndex != this->lastZIndex) {
        this->lastZIndex = this->zIndex;
        this->dirty = true;
    }

    if (this->getPosition() != this->lastPosition) {
        this->lastPosition = this->getPosition();
        this->dirty = true;
    }

    if (this->getSize() != this->lastSize) {
        this->lastSize = this->getSize();
        this->dirty = true;
    }

    if (this->getRotation() != this->lastRotation) {
        this->lastRotation = this->getRotation();
        this->dirty = true;
    }

}

bool SpriteRenderer::isDirty() {
    return this->dirty;
}

void SpriteRenderer::setClean() {
    this->dirty = false;
}
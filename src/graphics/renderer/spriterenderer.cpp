#include "graphics/renderer/spriterenderer.hpp"
#include "window/window.hpp"
#include "core/scene.hpp"
#include "util/assetpool.hpp"

SpriteRenderer::SpriteRenderer() : Component() {
    
    this->sprite = SpritePool::get("empty");
    this->colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->zIndex = 0;

    this->lastSprite = this->sprite;
    this->lastColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->lastZIndex = 0;

    this->dirty = true;

}

SpriteRenderer::~SpriteRenderer() {
    Window::getScene()->getRenderer()->remove(this);
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

Sprite* SpriteRenderer::getSprite() {
    return this->sprite;
}

vec4 SpriteRenderer::getColour() {
    return this->colour;
}

int SpriteRenderer::getZIndex() {
    return this->zIndex;
}

bool SpriteRenderer::isDirty() {
    return this->dirty;
}

SpriteRenderer* SpriteRenderer::setSprite(Sprite* sprite) {
    this->sprite = sprite;
    this->lastSprite = sprite;
    this->dirty = true;
    return this;
}

SpriteRenderer* SpriteRenderer::setColour(vec4 colour) {
    this->colour = colour;
    this->lastColour = colour;
    this->dirty = true;
    return this;
}

SpriteRenderer* SpriteRenderer::setZIndex(int zIndex) {
    this->zIndex = zIndex;
    this->lastZIndex = zIndex;
    this->dirty = true;
    return this;
}

SpriteRenderer* SpriteRenderer::setClean() {
    this->dirty = false;
    return this;
}
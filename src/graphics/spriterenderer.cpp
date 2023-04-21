#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/core/window.hpp"
#include "pancake/core/scene.hpp"
#include "pancake/asset/assetpool.hpp"

namespace Pancake {

    SpriteRenderer::SpriteRenderer() : TransformableComponent("SpriteRenderer") {
        
        this->sprite = SpritePool::get("empty");
        this->colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        this->zIndex = 0;

        this->lastSprite = this->sprite;
        this->lastColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        this->lastZIndex = 0;

        this->dirty = true;

    }

    void SpriteRenderer::end() {
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

    json SpriteRenderer::serialise() {

        json j = this->TransformableComponent::serialise();
        j.emplace("sprite", this->sprite->getName());

        j.emplace("colour", json::array());
        j["colour"].push_back(this->colour.x);
        j["colour"].push_back(this->colour.y);
        j["colour"].push_back(this->colour.z);
        j["colour"].push_back(this->colour.w);

        j.emplace("zIndex", this->zIndex);

        return j;

    }

    bool SpriteRenderer::load(json j) {
        
        if (!this->TransformableComponent::load(j)) {return false;}
        if (!j.contains("sprite") || !j["sprite"].is_string()) {return false;}

        if (!j.contains("colour") || !j["colour"].is_array()) {return false;}
        if (j["colour"].size() != 4) {return false;}
        if (!j["colour"][0].is_number()) {return false;}
        if (!j["colour"][1].is_number()) {return false;}
        if (!j["colour"][2].is_number()) {return false;}
        if (!j["colour"][3].is_number()) {return false;}
        
        if (!j.contains("zIndex") || !j["zIndex"].is_number_integer()) {return false;}

        Sprite* s = SpritePool::get(j["sprite"]);
        vec4 c = vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
        int z = j["zIndex"];

        this->setSprite(s);
        this->setColour(c);
        this->setZIndex(z);

        return true;

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

}
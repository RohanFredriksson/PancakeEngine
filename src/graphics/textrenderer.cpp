#include <string.h>
#include "pancake/graphics/textrenderer.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/asset/assetpool.hpp"

TextRenderer::TextRenderer() : TransformableComponent("TextRenderer") {
    
    this->text = "";
    this->font = FontPool::get("default");
    this->colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->zIndex = 0;

    this->lastText = "";
    this->lastFont = this->font;
    this->lastColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->lastZIndex = 0;
    this->lastPositionOffset = glm::vec2(0.0f, 0.0f);
    this->lastSizeScale = glm::vec2(0.0f, 0.0f);
    this->lastRotationOffset = 0.0f;

    this->dirty = true;

}

void TextRenderer::end() {
    int n = this->components.size();
    for (int i = 0; i < n; i++) {
        this->components[i]->kill();
    }
}

void TextRenderer::update(float dt) {

    if (this->text != this->lastText) {
        this->lastText = this->text;
        this->dirty = true;
    }

    if (this->font != this->lastFont) {
        this->lastFont = this->font;
        this->dirty = true;
    }

    if (this->colour != this->lastColour) {
        this->lastColour = this->colour;
        this->dirty = true;
    }

    if (this->getPositionOffset() != this->lastPositionOffset) {
        this->lastPositionOffset = this->getPositionOffset();
        this->dirty = true;
    }

    if (this->getSizeScale() != this->lastSizeScale) {
        this->lastSizeScale = this->getSizeScale();
        this->dirty = true;
    }

    if (this->getRotationOffset() != this->lastRotationOffset) {
        this->lastRotationOffset = this->getRotationOffset();
        this->dirty = true;
    }

    if (!dirty) {return;}

    int n = this->components.size();
    for (int i = 0; i < n; i++) {
        this->components[i]->kill();
    }
    this->components.clear();

    vec2 size = this->getSize();
    vec2 halfSize = size * 0.5f;
    vec2 min = this->getPosition() - halfSize;
    float scale = this->font->getScaleForHeight(size.y);

    float x = min.x;
    float y = min.y;

    const char* text = this->text.c_str();
    n = strlen(text);
    for (int i = 0; i < n; i++) {

        float width = scale * this->font->getAdvance(text[i]);
        float height = size.y;

        if (text[i] == '\n') {
            x = min.x;
            y -= size.y;
        }

        else {

            if (text[i] != ' ') {
                
                SpriteRenderer* s = new SpriteRenderer();
                s->setSerialisable(false);
                s->setSprite(this->font->getSprite(text[i]));
                s->setColour(this->colour);
                s->setZIndex(this->zIndex);

                vec2 positionOffset = glm::vec2(x + width * 0.5f, y + height * 0.5f) - this->getPosition();
                vec2 sizeScale = glm::vec2(width, height) / size;

                s->setPositionOffset(positionOffset);
                s->setSizeScale(sizeScale);
                s->setRotationOffset(-this->getEntity()->getRotation());

                this->getEntity()->addComponent(s);
                this->components.push_back(s);

            }
            x += width;
        }

    }
    this->dirty = false;
}

json TextRenderer::serialise() {

    json j = this->TransformableComponent::serialise();
    j.emplace("text", this->text);
    j.emplace("font", this->font->getFilename());

    j.emplace("colour", json::array());
    j["colour"].push_back(this->colour.x);
    j["colour"].push_back(this->colour.y);
    j["colour"].push_back(this->colour.z);
    j["colour"].push_back(this->colour.w);

    j.emplace("zIndex", this->zIndex);

    return j;

}

bool TextRenderer::load(json j) {
    
    if (!this->TransformableComponent::load(j)) {return false;}
    if (!j.contains("text") || !j["text"].is_string()) {return false;}
    if (!j.contains("font") || !j["font"].is_string()) {return false;}

    if (!j.contains("colour") || !j["colour"].is_array()) {return false;}
    if (j["colour"].size() != 4) {return false;}
    if (!j["colour"][0].is_number()) {return false;}
    if (!j["colour"][1].is_number()) {return false;}
    if (!j["colour"][2].is_number()) {return false;}
    if (!j["colour"][3].is_number()) {return false;}
    
    if (!j.contains("zIndex") || !j["zIndex"].is_number_integer()) {return false;}

    string t = j["text"];
    Font* f = FontPool::get(j["font"]);
    vec4 c = vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
    int z = j["zIndex"];

    this->setText(t);
    this->setFont(f);
    this->setColour(c);
    this->setZIndex(z);

    return true;

}

string TextRenderer::getText() {
    return this->text;
}

Font* TextRenderer::getFont() {
    return this->font;
}

vec4 TextRenderer::getColour() {
    return this->colour;
}

int TextRenderer::getZIndex() {
    return this->zIndex;
}

bool TextRenderer::isDirty() {
    return this->dirty;
}

TextRenderer* TextRenderer::setText(string text) {
    this->text = text;
    this->lastText = text;
    this->dirty = true;
    return this;
}

TextRenderer* TextRenderer::setFont(Font* font) {
    this->font = font;
    this->lastFont = font;
    this->dirty = true;
    return this;
}

TextRenderer* TextRenderer::setColour(vec4 colour) {
    this->colour = colour;
    this->lastColour = colour;
    this->dirty = true;
    return this;
}

TextRenderer* TextRenderer::setZIndex(int zIndex) {
    this->zIndex = zIndex;
    this->lastZIndex = zIndex;
    this->dirty = true;
    return this;
}

TextRenderer* TextRenderer::setClean() {
    this->dirty = false;
    return this;
}
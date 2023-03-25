#include <string.h>
#include "pancake/graphics/renderer/textrenderer.hpp"
#include "pancake/graphics/renderer/spriterenderer.hpp"
#include "pancake/util/assetpool.hpp"

TextRenderer::TextRenderer() : TransformableComponent() {
    
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

TextRenderer::~TextRenderer() {
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
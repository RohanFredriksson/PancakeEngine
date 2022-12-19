#include <string.h>
#include "graphics/renderer/textrenderer.hpp"
#include "graphics/renderer/spriterenderer.hpp"

TextRenderer::TextRenderer(string text, Font* font, vec4 colour, int zIndex) : Component() {
    
    this->text = text;
    this->font = font;
    this->colour = colour;
    this->zIndex = zIndex;

    this->lastText = text;
    this->lastFont = font;
    this->lastColour = colour;
    this->lastZIndex = zIndex;
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

void TextRenderer::setText(string text) {
    this->text = text;
    this->dirty = true;
}

void TextRenderer::setFont(Font* font) {
    this->font = font;
    this->dirty = true;
}

void TextRenderer::setColour(vec4 colour) {
    this->colour = colour;
    this->dirty = true;
}

void TextRenderer::setZIndex(int zIndex) {
    this->zIndex = zIndex;
    this->dirty = true;
}

void TextRenderer::setClean() {
    this->dirty = false;
}
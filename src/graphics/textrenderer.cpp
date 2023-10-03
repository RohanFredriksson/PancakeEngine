#include <algorithm>
#include <sstream>
#include <limits>

#include "pancake/graphics/textrenderer.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/core/engine.hpp"
#include "pancake/asset/assetpool.hpp"

namespace Pancake {

    TextRenderer::TextRenderer() : TransformableComponent("TextRenderer") {
        
        this->text = "";
        this->font = FontPool::get("default");
        this->colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        this->zIndex = 0;
        this->alignment = LEFT;

        this->lastText = this->text;
        this->lastFont = this->font;
        this->lastColour = this->colour;
        this->lastZIndex = this->zIndex;
        this->lastAlignment = this->alignment;
        this->lastPositionOffset = glm::vec2(0.0f, 0.0f);
        this->lastSizeScale = glm::vec2(0.0f, 0.0f);
        this->lastRotationOffset = 0.0f;

        this->dirty = true;

    }

    void TextRenderer::end() {
        for (SpriteRenderer* s : this->components) {
            if (s != nullptr) {
                s->kill();
            }
        }
    }

    float TextRenderer::width() {

        std::istringstream iss(this->text);
        std::string line;
        float scale = this->font->getScaleForHeight(this->getSize().y);
        float best = 0.0f;

        while (std::getline(iss, line)) {
            float current = 0.0f;
            for (char character : line) {current += scale * this->font->getAdvance(character);}
            if (current > best) {best = current;}
        }

        return best;
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

        if (this->zIndex != this->lastZIndex) {
            this->lastZIndex = this->zIndex;
            this->dirty = true;
        }

        if (this->alignment != this->lastAlignment) {
            this->lastAlignment = this->alignment;
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

        if (!this->dirty) {return;}

        for (SpriteRenderer* s : this->components) {s->kill();}
        this->components.clear();

        glm::vec2 size = this->getSize();
        glm::vec2 halfSize = size * 0.5f;
        glm::vec2 min = this->getPosition() - halfSize;
        glm::vec2 max = this->getPosition() + halfSize;
        float scale = this->font->getScaleForHeight(size.y);

        std::string t = std::string(this->text);
        if (this->alignment == RIGHT) {std::reverse(t.begin(), t.end());}

        float x = (this->alignment == RIGHT ? max.x : min.x);
        float y = min.y;

        for (char character : t) {

            float w = scale * this->font->getAdvance(character);
            float h = size.y;

            if (character == '\n') {
                x = (this->alignment == RIGHT ? max.x : min.x);
                y -= size.y;
                continue;
            }

            if (this->alignment == RIGHT) {x -= w;}

            if (character != ' ') {
                
                SpriteRenderer* s = new SpriteRenderer();
                s->setSerialisable(false);
                s->setImguiable(false);
                s->setSprite(this->font->getSprite(character));
                s->setColour(this->colour);
                s->setZIndex(this->zIndex);

                vec2 positionOffset = glm::vec2(x + w * 0.5f, y + h * 0.5f) - this->getPosition() + this->getPositionOffset();
                vec2 sizeScale = glm::vec2(w, h) / size;

                s->setPositionOffset(positionOffset);
                s->setSizeScale(sizeScale);
                s->setRotationOffset(-this->getEntity()->getRotation());

                this->getEntity()->addComponent(s);
                this->components.push_back(s);

            }

            if (this->alignment != RIGHT) {x += w;}

        }

        if (this->alignment == CENTER) {
            float offset = 0.5f * (this->getSize().x - this->width());
            for (SpriteRenderer* s : this->components) {
                s->addPositionOffset(offset, 0.0f);
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
        j.emplace("alignment", this->alignment);

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
        if (!j.contains("alignment") || !j["alignment"].is_number_integer()) {return false;}

        std::string t = j["text"];
        Font* f = FontPool::get(j["font"]);
        glm::vec4 c = vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
        int z = j["zIndex"];
        int a = j["alignment"];

        this->setText(t);
        this->setFont(f);
        this->setColour(c);
        this->setZIndex(z);
        this->setAlignment(a);

        return true;

    }

    void TextRenderer::imgui() {

        TransformableComponent::imgui();

        // Text
        char s[256];
        strncpy(s, this->text.c_str(), 256);
        s[255] = '\0';
        ImGui::Text("Text:           ");
        ImGui::SameLine();
        if (ImGui::InputText("##Text", s, sizeof(s))) {this->setText(string(s));}

        // Font

        // Colour
        glm::vec4 c = glm::vec4(this->colour.x, this->colour.y, this->colour.z, this->colour.w);
        ImGui::Text("Colour:         ");
        ImGui::SameLine();
        if (ImGui::DragFloat4("##Colour", &c[0])) {this->setColour(c);}

        // Z Index
        int z = this->zIndex;
        ImGui::Text("Z Index:        ");
        ImGui::SameLine();
        if (ImGui::DragInt("##ZIndex", &z)) {this->setZIndex(z);}

        // Alignment


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

    int TextRenderer::getAlignment() {
        return this->alignment;
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

    TextRenderer* TextRenderer::setColour(float r, float g, float b, float a) {
        return this->setColour(vec4(r, g, b, a));
    }

    TextRenderer* TextRenderer::setZIndex(int zIndex) {
        this->zIndex = zIndex;
        this->lastZIndex = zIndex;
        this->dirty = true;
        return this;
    }

    TextRenderer* TextRenderer::setAlignment(int alignment) {
        if (alignment < 0 || alignment > 2) {return this;}
        this->alignment = alignment;
        this->lastAlignment = alignment;
        this->dirty = true;
        return this;
    }

    TextRenderer* TextRenderer::setClean() {
        this->dirty = false;
        return this;
    }

}
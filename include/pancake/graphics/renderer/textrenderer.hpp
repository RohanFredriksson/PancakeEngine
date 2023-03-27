#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "pancake/core/component.hpp"
#include "pancake/graphics/primitives/font.hpp"
#include "pancake/graphics/primitives/sprite.hpp"

using std::string;
using glm::vec2;
using glm::vec4;

class TextRenderer : public TransformableComponent {

    private:

        string text;
        Font* font;
        vec4 colour;
        int zIndex;

        vector<Component*> components;

        string lastText;
        Font* lastFont;
        vec4 lastColour;
        vec2 lastPositionOffset;
        vec2 lastSizeScale;
        float lastRotationOffset;
        int lastZIndex;
        
        bool dirty;

    public:

        static Component* create();
        TextRenderer();
        ~TextRenderer() override;
        void update(float dt) override;
        json serialise() override;

        // Getters
        string getText();
        Font* getFont();
        vec4 getColour();
        int getZIndex();
        bool isDirty();
        
        // Setters
        TextRenderer* setText(string text);
        TextRenderer* setFont(Font* font);
        TextRenderer* setColour(vec4 colour);
        TextRenderer* setZIndex(int zIndex);
        TextRenderer* setClean();

};
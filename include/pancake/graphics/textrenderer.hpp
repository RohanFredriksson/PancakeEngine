#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "pancake/core/component.hpp"
#include "pancake/graphics/font.hpp"
#include "pancake/graphics/sprite.hpp"
#include "pancake/graphics/spriterenderer.hpp"

namespace Pancake {

    class TextRenderer : public TransformableComponent {

        private:

            std::string text;
            Font* font;
            glm::vec4 colour;
            int zIndex;
            int alignment;

            std::vector<SpriteRenderer*> components;

            std::string lastText;
            Font* lastFont;
            glm::vec4 lastColour;
            glm::vec2 lastPositionOffset;
            glm::vec2 lastSizeScale;
            float lastRotationOffset;
            int lastZIndex;
            int lastAlignment;
            
            bool dirty;

            float width();
            
        public:

            enum Alignment {
                LEFT = 0,
                CENTER = 1,
                RIGHT = 2
            };

            TextRenderer();
            void end() override;
            void update(float dt) override;
            json serialise() override;
            bool load(json j) override;
            void imgui() override;

            // Getters
            std::string getText();
            Font* getFont();
            glm::vec4 getColour();
            int getZIndex();
            int getAlignment();
            bool isDirty();
            
            // Setters
            TextRenderer* setText(string text);
            TextRenderer* setFont(Font* font);
            TextRenderer* setColour(glm::vec4 colour);
            TextRenderer* setColour(float r, float g, float b, float a);
            TextRenderer* setZIndex(int zIndex);
            TextRenderer* setAlignment(int alignment);
            TextRenderer* setClean();

    };

    REGISTER(Component, TextRenderer);

}
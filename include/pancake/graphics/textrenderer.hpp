#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "pancake/core/component.hpp"
#include "pancake/graphics/font.hpp"
#include "pancake/graphics/sprite.hpp"

using std::string;
using glm::vec2;
using glm::vec4;

namespace Pancake {

    class TextRenderer : public TransformableComponent {

        private:

            string text;
            Font* font;
            vec4 colour;
            int zIndex;
            int alignment;

            vector<int> components;

            string lastText;
            Font* lastFont;
            vec4 lastColour;
            vec2 lastPositionOffset;
            vec2 lastSizeScale;
            float lastRotationOffset;
            int lastZIndex;
            int lastAlignment;
            
            bool dirty;

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
            string getText();
            Font* getFont();
            vec4 getColour();
            int getZIndex();
            int getAlignment();
            bool isDirty();
            
            // Setters
            TextRenderer* setText(string text);
            TextRenderer* setFont(Font* font);
            TextRenderer* setColour(vec4 colour);
            TextRenderer* setColour(float r, float g, float b, float a);
            TextRenderer* setZIndex(int zIndex);
            TextRenderer* setAlignment(int alignment);
            TextRenderer* setClean();

    };

    REGISTER(Component, TextRenderer);

}
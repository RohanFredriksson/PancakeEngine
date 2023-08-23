#pragma once

#include <glm/glm.hpp>
#include "pancake/core/component.hpp"
#include "pancake/graphics/sprite.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace Pancake {

    class SpriteRenderer : public TransformableComponent {

        private:

            Sprite* sprite;
            vec4 colour;
            int zIndex;

            Sprite* lastSprite;
            vec4 lastColour;
            int lastZIndex;
            vec2 lastPosition;
            vec2 lastSize;
            float lastRotation;
            
            bool dirty;

        public:

            SpriteRenderer();
            void start() override;
            void end() override;
            void update(float dt) override;
            json serialise() override;
            bool load(json j) override;
            void imgui() override;
            
            // Getters
            Sprite* getSprite();
            vec4 getColour();
            int getZIndex();
            bool isDirty();
            
            // Setters
            SpriteRenderer* setSprite(string sprite);
            SpriteRenderer* setSprite(Sprite* sprite);
            SpriteRenderer* setColour(vec4 colour);
            SpriteRenderer* setColour(vec3 colour);
            SpriteRenderer* setColour(float r, float g, float b, float a);
            SpriteRenderer* setColour(float r, float g, float b);
            SpriteRenderer* setZIndex(int zIndex);
            SpriteRenderer* setClean();

    };

    REGISTER(Component, SpriteRenderer);

}
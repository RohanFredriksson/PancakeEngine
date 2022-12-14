#pragma once

#include <glm/glm.hpp>
#include "core/component.hpp"
#include "graphics/primitives/sprite.hpp"

using glm::vec2;
using glm::vec4;

class SpriteRenderer : public Component {

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
        ~SpriteRenderer() override;

        void update(float dt) override;
        
        // Getters
        Sprite* getSprite();
        vec4 getColour();
        int getZIndex();
        bool isDirty();
        
        // Setters
        SpriteRenderer* setSprite(Sprite* sprite);
        SpriteRenderer* setColour(vec4 colour);
        SpriteRenderer* setZIndex(int zIndex);
        SpriteRenderer* setClean();

};
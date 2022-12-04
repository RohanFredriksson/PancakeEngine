#pragma once

#include <glm/glm.hpp>
#include "core/entity.hpp"
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

        SpriteRenderer(Sprite* sprite, vec4 colour, int zIndex);
        void update(float dt) override;
        
        bool isDirty();
        void setClean();

};
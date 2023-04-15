#pragma once

#include "pancake/components/transition.hpp"
#include "pancake/graphics/spriterenderer.hpp"

class FadeFromBlack : public Transition {

    private:

        SpriteRenderer* spriterenderer;
        float duration;
        float time;

    public:

        FadeFromBlack();
        void start() override;
        void update(float dt) override;
        json serialise() override;
        bool load(json j) override;

        void clean() override;

        void setDuration(float duration);
        float getDuration();

};
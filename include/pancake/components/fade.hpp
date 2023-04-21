#pragma once

#include "pancake/components/transition.hpp"
#include "pancake/graphics/spriterenderer.hpp"

namespace Pancake {

    class FadeFromBlack : public Transition {

        private:

            SpriteRenderer* spriterenderer;
            float duration;
            float time;

        public:

            FadeFromBlack();
            void start() override;
            void end() override;
            void update(float dt) override;
            json serialise() override;
            bool load(json j) override;

            void setDuration(float duration);
            float getDuration();

    };

    class FadeToBlack : public Transition {

        private:

            SpriteRenderer* spriterenderer;
            float duration;
            float time;

        public:

            FadeToBlack();
            void start() override;
            void end() override;
            void update(float dt) override;
            json serialise() override;
            bool load(json j) override;

            void setDuration(float duration);
            float getDuration();

    };

}
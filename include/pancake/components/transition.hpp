#pragma once

#include "pancake/core/component.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include <glm/glm.hpp>

using glm::vec4;

namespace Pancake {

    class Transition : public Component {

        public:

            Transition(string type);

            void flag();
            virtual void onFlag();
            
    };

    class FadeTransition : public Transition {

        private:

            SpriteRenderer* spriterenderer;
            float duration;
            float time;
            vec4 from;
            vec4 to;
            vec4 diff;

        public:

            FadeTransition();
            FadeTransition(string type);
            void start() override;
            void end() override;
            void update(float dt) override;
            json serialise() override;
            bool load(json j) override;
            
            void setDuration(float duration);
            void setTime(float time);
            void setFrom(vec4 colour);
            void setTo(vec4 colour);
            
            float getDuration();
            float getTime();
            vec4 getFrom();
            vec4 getTo();

    };

    REGISTER(Component, FadeTransition);

}
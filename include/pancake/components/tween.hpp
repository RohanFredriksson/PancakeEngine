#pragma once

#include <functional>
#include <glm/glm.hpp>

#include "pancake/core/component.hpp"
#include "pancake/graphics/spriterenderer.hpp"

namespace Pancake {

    // Easing functions map progress in [0, 1] to eased progress, starting at 0 and ending at 1.
    namespace Ease {

        typedef float (*Function)(float t);

        float linear(float t);
        float inQuad(float t);
        float outQuad(float t);
        float inOutQuad(float t);
        float inCubic(float t);
        float outCubic(float t);
        float inOutCubic(float t);
        float inSine(float t);
        float outSine(float t);
        float inOutSine(float t);
        float outBack(float t);
        float outBounce(float t);

    }

    // Animates a value over time, then kills itself. It advances with the scene's delta time, so it follows
    // the time scale and stops while paused. Tweens live on an entity, so they die with it, and are not saved.
    // Start values are read when the tween begins, after its delay, so delayed tweens can be chained.
    class Tween : public Component {

        private:

            float duration;
            float delay;
            float elapsed;
            bool begun;
            bool finished;
            Ease::Function ease;
            std::function<void()> beginCallback;
            std::function<void(float)> applyCallback;
            std::function<void()> completeCallback;

        public:

            Tween();
            Tween(float duration, std::function<void(float)> apply, Ease::Function ease = Ease::linear);

            void update(float dt) override;
            void imgui() override;

            float getDuration();
            float getDelay();
            float getProgress();
            bool isFinished();

            Tween* setDelay(float delay);
            Tween* setEase(Ease::Function ease);
            Tween* onBegin(std::function<void()> callback);
            Tween* onComplete(std::function<void()> callback);

            // Tween an entity's transform, or a sprite's colour. The tween is added to the entity that owns the target.
            static Tween* position(Entity* entity, glm::vec2 to, float duration, Ease::Function ease = Ease::linear);
            static Tween* size(Entity* entity, glm::vec2 to, float duration, Ease::Function ease = Ease::linear);
            static Tween* rotation(Entity* entity, float to, float duration, Ease::Function ease = Ease::linear);
            static Tween* colour(SpriteRenderer* sprite, glm::vec4 to, float duration, Ease::Function ease = Ease::linear);

            // Call apply with eased progress from 0 to 1, for anything else.
            static Tween* value(Entity* entity, float duration, std::function<void(float)> apply, Ease::Function ease = Ease::linear);

    };

}

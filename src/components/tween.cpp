#include <cmath>
#include <memory>
#include <iostream>
#include <algorithm>
#include <imgui.h>

#include "pancake/components/tween.hpp"

namespace Pancake {

    namespace {
        const float PI = 3.14159265358979f;
    }

    namespace Ease {

        float linear(float t) {return t;}
        float inQuad(float t) {return t * t;}
        float outQuad(float t) {return 1.0f - (1.0f - t) * (1.0f - t);}
        float inOutQuad(float t) {return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);}
        float inCubic(float t) {return t * t * t;}
        float outCubic(float t) {return 1.0f - std::pow(1.0f - t, 3.0f);}
        float inOutCubic(float t) {return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(2.0f - 2.0f * t, 3.0f) / 2.0f;}
        float inSine(float t) {return 1.0f - std::cos(t * PI / 2.0f);}
        float outSine(float t) {return std::sin(t * PI / 2.0f);}
        float inOutSine(float t) {return 0.5f - std::cos(t * PI) / 2.0f;}

        float outBack(float t) {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
        }

        float outBounce(float t) {
            const float n = 7.5625f;
            const float d = 2.75f;
            if (t < 1.0f / d) {return n * t * t;}
            if (t < 2.0f / d) {t -= 1.5f / d; return n * t * t + 0.75f;}
            if (t < 2.5f / d) {t -= 2.25f / d; return n * t * t + 0.9375f;}
            t -= 2.625f / d;
            return n * t * t + 0.984375f;
        }

    }

    Tween::Tween() : Tween(0.0f, nullptr) {

    }

    Tween::Tween(float duration, std::function<void(float)> apply, Ease::Function ease) : Component("Tween") {
        this->duration = duration;
        this->delay = 0.0f;
        this->elapsed = 0.0f;
        this->begun = false;
        this->finished = false;
        this->ease = ease == nullptr ? Ease::linear : ease;
        this->applyCallback = apply;
        this->setSerialisable(false);
    }

    void Tween::update(float dt) {

        if (this->finished) {return;}

        // Wait out the delay, carrying any leftover time into the tween.
        if (this->delay > 0.0f) {
            this->delay -= dt;
            if (this->delay > 0.0f) {return;}
            dt = -this->delay;
            this->delay = 0.0f;
        }

        if (!this->begun) {
            this->begun = true;
            if (this->beginCallback) {this->beginCallback();}
        }

        this->elapsed += dt;
        float t = this->getProgress();
        if (this->applyCallback) {this->applyCallback(t >= 1.0f ? 1.0f : this->ease(t));}

        if (t >= 1.0f) {
            this->finished = true;
            this->kill();
            if (this->completeCallback) {this->completeCallback();}
        }

    }

    void Tween::imgui() {
        ImGui::ProgressBar(this->getProgress());
    }

    float Tween::getDuration() {
        return this->duration;
    }

    float Tween::getDelay() {
        return this->delay;
    }

    float Tween::getProgress() {
        if (this->duration <= 0.0f) {return this->begun ? 1.0f : 0.0f;}
        return std::min(this->elapsed / this->duration, 1.0f);
    }

    bool Tween::isFinished() {
        return this->finished;
    }

    Tween* Tween::setDelay(float delay) {
        this->delay = delay;
        return this;
    }

    Tween* Tween::setEase(Ease::Function ease) {
        this->ease = ease == nullptr ? Ease::linear : ease;
        return this;
    }

    Tween* Tween::onBegin(std::function<void()> callback) {
        this->beginCallback = callback;
        return this;
    }

    Tween* Tween::onComplete(std::function<void()> callback) {
        this->completeCallback = callback;
        return this;
    }

    Tween* Tween::position(Entity* entity, glm::vec2 to, float duration, Ease::Function ease) {
        std::shared_ptr<glm::vec2> from = std::make_shared<glm::vec2>();
        Tween* tween = new Tween(duration, [entity, from, to](float t) {entity->setPosition(glm::mix(*from, to, t));}, ease);
        tween->beginCallback = [entity, from]() {*from = entity->getPosition();};
        entity->addComponent(tween);
        return tween;
    }

    Tween* Tween::size(Entity* entity, glm::vec2 to, float duration, Ease::Function ease) {
        std::shared_ptr<glm::vec2> from = std::make_shared<glm::vec2>();
        Tween* tween = new Tween(duration, [entity, from, to](float t) {entity->setSize(glm::mix(*from, to, t));}, ease);
        tween->beginCallback = [entity, from]() {*from = entity->getSize();};
        entity->addComponent(tween);
        return tween;
    }

    Tween* Tween::rotation(Entity* entity, float to, float duration, Ease::Function ease) {
        std::shared_ptr<float> from = std::make_shared<float>();
        // Rotate by the difference, so transformable components' offsets rotate with the entity.
        Tween* tween = new Tween(duration, [entity, from, to](float t) {entity->addRotation(glm::mix(*from, to, t) - entity->getRotation());}, ease);
        tween->beginCallback = [entity, from]() {*from = entity->getRotation();};
        entity->addComponent(tween);
        return tween;
    }

    Tween* Tween::colour(SpriteRenderer* sprite, glm::vec4 to, float duration, Ease::Function ease) {

        // The sprite may be removed before the tween ends, so look it up by id rather than keeping the pointer.
        Entity* entity = sprite->getEntity();
        if (entity == nullptr) {std::cout << "ERROR::TWEEN::COLOUR::SPRITE_HAS_NO_ENTITY\n"; return nullptr;}
        int id = sprite->getId();
        auto find = [entity, id]() -> SpriteRenderer* {
            for (Component* c : entity->getComponents()) {
                if (c->getId() == id && !c->isDead()) {return dynamic_cast<SpriteRenderer*>(c);}
            }
            return nullptr;
        };

        std::shared_ptr<glm::vec4> from = std::make_shared<glm::vec4>();
        Tween* tween = new Tween(duration, [find, from, to](float t) {
            SpriteRenderer* s = find();
            if (s != nullptr) {s->setColour(glm::mix(*from, to, t));}
        }, ease);
        tween->beginCallback = [find, from]() {
            SpriteRenderer* s = find();
            if (s != nullptr) {*from = s->getColour();}
        };
        entity->addComponent(tween);
        return tween;

    }

    Tween* Tween::value(Entity* entity, float duration, std::function<void(float)> apply, Ease::Function ease) {
        Tween* tween = new Tween(duration, apply, ease);
        entity->addComponent(tween);
        return tween;
    }

}

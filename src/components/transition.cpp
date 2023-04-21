#include "pancake/components/transition.hpp"

namespace Pancake {

    Transition::Transition(string type) : Component(type) {
        
    }

    void Transition::flag() {
        this->onFlag();
        this->kill();
    }

    void Transition::onFlag() {

    }

    FadeTransition::FadeTransition() : Transition("FadeTransition") {

        this->spriterenderer = nullptr;
        this->duration = 0.0f;
        this->time = 0.0f;
        this->from = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        this->to   = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        this->diff = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    }

    FadeTransition::FadeTransition(string type) : Transition(type) {
        
        this->spriterenderer = nullptr;
        this->duration = 0.0f;
        this->time = 0.0f;
        this->from = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        this->to   = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        this->diff = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    }

    void FadeTransition::start() {

        this->spriterenderer = new SpriteRenderer();
        this->spriterenderer->setSerialisable(false);
        this->spriterenderer->setSizeScale(vec2(10000.0f, 10000.0f));
        this->spriterenderer->setColour(this->from);
        this->spriterenderer->setZIndex(1000);
        this->getEntity()->addComponent(this->spriterenderer);

    }

    void FadeTransition::end() {
        this->spriterenderer->kill();
    }

    void FadeTransition::update(float dt) {
        
        this->time += dt;
        this->spriterenderer->setColour(this->from + (this->time / this->duration) * this->diff);
        
        if (this->time >= this->duration) {
            this->spriterenderer->setColour(this->to);    
            this->flag();
        }

    }

    json FadeTransition::serialise() {
        
        json j = this->Component::serialise();
        j.emplace("duration", this->duration);
        j.emplace("time", this->time);

        j.emplace("from", json::array());
        j["from"].push_back(this->from.x);
        j["from"].push_back(this->from.y);
        j["from"].push_back(this->from.z);
        j["from"].push_back(this->from.w);

        j.emplace("to", json::array());
        j["to"].push_back(this->to.x);
        j["to"].push_back(this->to.y);
        j["to"].push_back(this->to.z);
        j["to"].push_back(this->to.w);

        return j;

    }

    bool FadeTransition::load(json j) {

        if (!this->Component::load(j)) {return false;}
        if (!j.contains("duration") || !j["duration"].is_number()) {return false;}
        if (!j.contains("time") || !j["time"].is_number()) {return false;}

        if (!j.contains("from") || !j["from"].is_array()) {return false;}
        if (j["from"].size() != 4) {return false;}
        if (!j["from"][0].is_number()) {return false;}
        if (!j["from"][1].is_number()) {return false;}
        if (!j["from"][2].is_number()) {return false;}
        if (!j["from"][3].is_number()) {return false;}

        if (!j.contains("to") || !j["to"].is_array()) {return false;}
        if (j["to"].size() != 4) {return false;}
        if (!j["to"][0].is_number()) {return false;}
        if (!j["to"][1].is_number()) {return false;}
        if (!j["to"][2].is_number()) {return false;}
        if (!j["to"][3].is_number()) {return false;}

        this->setDuration(j["duration"]);
        this->setTime(j["time"]);
        this->setFrom(vec4(j["from"][0], j["from"][1], j["from"][2], j["from"][3]));
        this->setTo(vec4(j["to"][0], j["to"][1], j["to"][2], j["to"][3]));

        return true;

    }

    void FadeTransition::setDuration(float duration) {
        this->duration = duration;
    }

    void FadeTransition::setTime(float time) {
        this->time = time;
    }

    void FadeTransition::setFrom(vec4 colour) {
        this->from = colour;
        this->diff = this->to - this->from;
    }
    
    void FadeTransition::setTo(vec4 colour) {
        this->to = colour;
        this->diff = this->to - this->from;
    }
    
    float FadeTransition::getDuration() {
        return this->duration;
    }
    
    float FadeTransition::getTime() {
        return this->time;
    }
    
    vec4 FadeTransition::getFrom() {
        return this->from;
    }
    
    vec4 FadeTransition::getTo() {
        return this->to;
    }

}
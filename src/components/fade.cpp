#include <algorithm>
#include "pancake/components/fade.hpp"

FadeFromBlack::FadeFromBlack() : Transition("FadeFromBlack") {
    this->spriterenderer = nullptr;
    this->duration = 0.0f;
    this->time = 0.0f;
}

void FadeFromBlack::start() {
    this->spriterenderer = new SpriteRenderer();
    this->spriterenderer->setSerialisable(false);
    this->spriterenderer->setSizeScale(vec2(10000.0f, 10000.0f));
    this->spriterenderer->setColour(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    this->spriterenderer->setZIndex(1000);
    this->getEntity()->addComponent(this->spriterenderer);
}

void FadeFromBlack::end() {
    this->spriterenderer->kill();
}

void FadeFromBlack::update(float dt) {
    this->time += dt;
    this->spriterenderer->setColour(vec4(0.0f, 0.0f, 0.0f, std::max(0.0f, 1.0f - (this->time / this->duration))));
    if (this->time >= this->duration) {this->flag();}
}

json FadeFromBlack::serialise() {
    json j = this->Component::serialise();
    j.emplace("duration", this->duration);
    j.emplace("time", this->time);
    return j;
}

bool FadeFromBlack::load(json j) {
    if (!this->Component::load(j)) {return false;}
    if (!j.contains("duration") || !j["duration"].is_number()) {return false;}
    if (!j.contains("time") || !j["time"].is_number()) {return false;}
    this->duration = j["duration"];
    this->time = j["time"];
    return true;
}

void FadeFromBlack::setDuration(float duration) {
    this->duration = duration;
}

float FadeFromBlack::getDuration() {
    return this->duration;
}

FadeToBlack::FadeToBlack() : Transition("FadeToBlack") {
    this->spriterenderer = nullptr;
    this->duration = 0.0f;
    this->time = 0.0f;
}

void FadeToBlack::start() {
    this->spriterenderer = new SpriteRenderer();
    this->spriterenderer->setSerialisable(false);
    this->spriterenderer->setSizeScale(vec2(10000.0f, 10000.0f));
    this->spriterenderer->setColour(vec4(0.0f, 0.0f, 0.0f, 0.0f));
    this->spriterenderer->setZIndex(1000);
    this->getEntity()->addComponent(this->spriterenderer);
}

void FadeToBlack::end() {
    this->spriterenderer->kill();
}

void FadeToBlack::update(float dt) {
    this->time += dt;
    this->spriterenderer->setColour(vec4(0.0f, 0.0f, 0.0f, std::min(1.0f, this->time / this->duration)));
    if (this->time >= this->duration) {this->flag();}
}

json FadeToBlack::serialise() {
    json j = this->Component::serialise();
    j.emplace("duration", this->duration);
    j.emplace("time", this->time);
    return j;
}

bool FadeToBlack::load(json j) {
    if (!this->Component::load(j)) {return false;}
    if (!j.contains("duration") || !j["duration"].is_number()) {return false;}
    if (!j.contains("time") || !j["time"].is_number()) {return false;}
    this->duration = j["duration"];
    this->time = j["time"];
    return true;
}

void FadeToBlack::setDuration(float duration) {
    this->duration = duration;
}

float FadeToBlack::getDuration() {
    return this->duration;
}
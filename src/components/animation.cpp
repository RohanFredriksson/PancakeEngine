#include "pancake/components/animation.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/asset/assetpool.hpp"

#include <utility>
using std::pair;

AnimationState::AnimationState() {
    this->title = "";
    this->current = 0;
    this->time = 0.0f;
    this->loop = false;
}

void AnimationState::update(float dt) {

    // If the frame is already out of bounds don't bother incrementing the frame.
    if (current >= this->frames.size()) {return;}
    this->time -= dt;

    // If the frame has not been completed we don't need to change frames.
    if (this->time > 0) {return;} 
    
    // If the frame is not the last or we can loop, increment the frame.
    if (this->current != this->frames.size() - 1 || this->loop) {this->current = (this->current + 1) % this->frames.size();}
    this->time = this->frames[this->current].duration;

}

//json AnimationState::serialise() {}
//AnimationState* AnimationState::load(json j) {}

void AnimationState::setTitle(string title) {
    this->title = title;
}

void AnimationState::setCurrent(int current) {
    this->current = current;
}

void AnimationState::setTime(float time) {
    this->time = time;
}

void AnimationState::setLoop(bool loop) {
    this->loop = loop;
}

void AnimationState::addFrame(Sprite* sprite, float duration) {
    AnimationFrame frame = {sprite, duration};
    this->frames.push_back(frame);
}

void AnimationState::addFrames(vector<Sprite*> sprites, float duration) {
    for (Sprite* sprite : sprites) {
        AnimationFrame frame = {sprite, duration};
        this->frames.push_back(frame);
    }
}

string AnimationState::getTitle() {
    return this->title;
}

Sprite* AnimationState::getCurrentSprite() {
    if (this->current < this->frames.size()) {return this->frames[this->current].sprite;}
    return SpritePool::get("empty");
}

bool AnimationState::doesLoop() {
    return this->loop;
}

Animation::Animation() : Component("Animation") {
    this->current = nullptr;
    this->defaultState = "";
}

void Animation::start() {
    auto search = this->states.find(this->defaultState);
    if (search != this->states.end()) {this->current = search->second;}
}

void Animation::end()  {
    this->clearStates();
}

void Animation::update(float dt)  {
    if (this->current == nullptr) {return;}
    this->current->update(dt);
    SpriteRenderer* spriterenderer = dynamic_cast<SpriteRenderer*>(this->getEntity()->getComponentByType("SpriteRenderer"));
    if (spriterenderer == nullptr) {return;}
    spriterenderer->setSprite(this->current->getCurrentSprite());
}

json Animation::serialise()  {
    json j;
    return j;
}

bool Animation::load(json j)  {
    return false;
}

void Animation::setDefaultState(string title)  {

    auto search = this->states.find(title);
    if (search != this->states.end()) {
        this->defaultState = title;
        if (this->current == nullptr) {this->current = search->second;}
    }

}

void Animation::addTransfer(string from, string to, string trigger)  {
    tuple<string, string> key(from, trigger);
    pair<tuple<string, string>, string> p(key, to);
    this->transfers.insert(p);
}

void Animation::addState(AnimationState* state) {
    pair<string, AnimationState*> p(state->getTitle(), state);
    this->states.insert(p);
}

void Animation::clearStates() {
    for (auto const& x: this->states) {
        AnimationState* a = x.second;
        delete a;
    }
}

void Animation::trigger(string trigger)  {
    
    // If we are at the null state, we can't transfer to any other states.
    if (this->current == nullptr) {return;}

    // Check if there is a transfer for this trigger on the current state.
    tuple<string, string> key(this->current->getTitle(), trigger);
    auto search1 = this->transfers.find(key);
    if (search1 == this->transfers.end()) {return;}
    
    // Find the next state.
    string next = search1->second;
    auto search2 = this->states.find(next);
    if (search2 == this->states.end()) {return;}
    this->current = search2->second;

}
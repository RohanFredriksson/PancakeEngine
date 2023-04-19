#include "pancake/components/animation.hpp"
#include "pancake/asset/assetpool.hpp"

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
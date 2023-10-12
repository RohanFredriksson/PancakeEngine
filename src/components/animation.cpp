#include "pancake/components/animation.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/asset/assetpool.hpp"

#include <utility>

namespace Pancake {

    AnimationFrame::AnimationFrame(Sprite* sprite, float duration) {
        this->sprite = sprite;
        this->duration = duration;
    }

    json AnimationFrame::serialise() {
        json j;
        j.emplace("sprite", this->sprite->getName());
        j.emplace("duration", this->duration);
        return j;
    }

    AnimationFrame* AnimationFrame::load(json j) {
        if (!j.contains("sprite") || !j["sprite"].is_string()) {return nullptr;}
        if (!j.contains("duration") || !j["duration"].is_number()) {return nullptr;}
        return new AnimationFrame(SpritePool::get(j["sprite"]), j["duration"]);
    }

    AnimationState::AnimationState() {
        this->title = "";
        this->current = 0;
        this->time = 0.0f;
        this->loop = false;
    }

    AnimationState::~AnimationState() {
        for (AnimationFrame* f : this->frames) {delete f;}
    }

    void AnimationState::update(float dt) {

        // If the frame is already out of bounds don't bother incrementing the frame.
        if (current >= this->frames.size()) {return;}
        this->time -= dt;

        // If the frame has not been completed we don't need to change frames.
        if (this->time > 0) {return;} 
        
        // If the frame is not the last or we can loop, increment the frame.
        if (this->current != this->frames.size() - 1 || this->loop) {this->current = (this->current + 1) % this->frames.size();}
        this->time = this->frames[this->current]->duration;

    }

    json AnimationState::serialise() {
        
        json j;
        j.emplace("title", this->title);
        j.emplace("current", this->current);
        j.emplace("time", this->time);
        j.emplace("loop", this->loop);
        
        j.emplace("frames", json::array());
        for (AnimationFrame* f : this->frames) {
            j["frames"].push_back(f->serialise());
        }

        return j;
    }

    AnimationState* AnimationState::load(json j) {

        if (!j.contains("title") || !j["title"].is_string()) {return nullptr;}
        if (!j.contains("current") || !j["current"].is_number_integer()) {return nullptr;}
        if (!j.contains("time") || !j["time"].is_number()) {return nullptr;}
        if (!j.contains("loop") || !j["loop"].is_boolean()) {return nullptr;}
        if (!j.contains("frames") || !j["frames"].is_array()) {return nullptr;}

        AnimationState* state = new AnimationState();
        state->setTitle(j["title"]);
        state->setCurrent(j["current"]);
        state->setTime(j["time"]);
        state->setLoop(j["loop"]);

        for (auto element : j["frames"]) {
            if (element.is_object()) {
                AnimationFrame* f = AnimationFrame::load(element);
                if (f != nullptr) {state->addFrame(f);}
            }
        }

        return state;
    }

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

    void AnimationState::addFrame(string sprite, float duration) {
        this->addFrame(SpritePool::get(sprite), duration);
    }

    void AnimationState::addFrame(Sprite* sprite, float duration) {
        AnimationFrame* frame = new AnimationFrame(sprite, duration);
        this->frames.push_back(frame);
    }

    void AnimationState::addFrame(AnimationFrame* frame) {
        this->frames.push_back(frame);
    }

    void AnimationState::addFrames(vector<string> sprites, float duration) {
        for (string sprite : sprites) {
            this->addFrame(sprite, duration);
        }
    }

    void AnimationState::addFrames(vector<Sprite*> sprites, float duration) {
        for (Sprite* sprite : sprites) {
            this->addFrame(sprite, duration);
        }
    }

    void AnimationState::addFrames(vector<AnimationFrame*> frames) {
        for (AnimationFrame* frame : frames) {
            this->frames.push_back(frame);
        }
    }

    string AnimationState::getTitle() {
        return this->title;
    }

    Sprite* AnimationState::getCurrentSprite() {
        if (this->current < this->frames.size()) {return this->frames[this->current]->sprite;}
        return SpritePool::get("empty");
    }

    bool AnimationState::doesLoop() {
        return this->loop;
    }

    Animation::Animation() : TransformableComponent("Animation") {

        this->currentState = nullptr;
        this->defaultState = "";
        this->spriterenderer = nullptr;

        this->colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        this->zIndex = 0;

        this->lastColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        this->lastZIndex = 0;
        this->lastPositionOffset = glm::vec2(0.0f, 0.0f);
        this->lastSizeScale = glm::vec2(0.0f, 0.0f);
        this->lastRotationOffset = 0.0f;

    }

    void Animation::start() {

        this->spriterenderer = new SpriteRenderer(); 
        this->spriterenderer->setSerialisable(false);
        this->spriterenderer->setColour(this->colour);
        this->spriterenderer->setZIndex(this->zIndex);
        this->spriterenderer->setPositionOffset(this->getPositionOffset());
        this->spriterenderer->setSizeScale(this->getSizeScale());
        this->spriterenderer->setRotationOffset(this->getRotationOffset());
        this->getEntity()->addComponent(this->spriterenderer);

        auto search = this->states.find(this->defaultState);
        if (search != this->states.end()) {this->currentState = search->second;}

    }

    void Animation::end()  {
        this->clearStates();
        if (this->spriterenderer != nullptr) {this->spriterenderer->kill();}
    }

    void Animation::update(float dt)  {

        // Find the spriterenderer component, if not exists, make one.
        if (this->spriterenderer == nullptr) {
            this->spriterenderer = new SpriteRenderer(); 
            this->spriterenderer->setSerialisable(false);
            this->spriterenderer->setImguiable(false);
            this->spriterenderer->setColour(this->colour);
            this->spriterenderer->setZIndex(this->zIndex);
            this->spriterenderer->setPositionOffset(this->getPositionOffset());
            this->spriterenderer->setSizeScale(this->getSizeScale());
            this->spriterenderer->setRotationOffset(this->getRotationOffset());
            this->getEntity()->addComponent(this->spriterenderer);
        }

        // Check if we need to update the spriterenderers attributes since it is linked to this object.
        if (this->colour != this->lastColour) {this->lastColour = this->colour; this->spriterenderer->setColour(this->colour);}
        if (this->zIndex != this->lastZIndex) {this->lastZIndex = this->zIndex; this->spriterenderer->setZIndex(this->zIndex);}
        if (this->getPositionOffset() != this->lastPositionOffset) {this->lastPositionOffset = this->getPositionOffset(); this->spriterenderer->setPositionOffset(this->getPositionOffset());}
        if (this->getSizeScale() != this->lastSizeScale) {this->lastSizeScale = this->getSizeScale(); this->spriterenderer->setSizeScale(this->getSizeScale());}
        if (this->getRotationOffset() != this->lastRotationOffset) {this->lastRotationOffset = this->getRotationOffset(); this->spriterenderer->setRotationOffset(this->getRotationOffset());}

        // Update the current state.
        if (this->currentState == nullptr) {return;}
        this->currentState->update(dt);
        this->spriterenderer->setSprite(this->currentState->getCurrentSprite());

    }

    json Animation::serialise()  {

        json j = this->TransformableComponent::serialise();
        j.emplace("defaultState", this->defaultState);
        string currentState = (this->currentState == nullptr ? "" : this->currentState->getTitle());
        j.emplace("currentState", currentState);

        j.emplace("colour", json::array());
        j["colour"].push_back(this->colour.x);
        j["colour"].push_back(this->colour.y);
        j["colour"].push_back(this->colour.z);
        j["colour"].push_back(this->colour.w);

        j.emplace("zIndex", this->zIndex);

        j.emplace("states", json::array());
        for (auto const& x: this->states) {
            AnimationState* a = x.second;
            j["states"].push_back(a->serialise());
        }

        j.emplace("transfers", json::array());
        for (auto const& x: this->transfers) {
            
            tuple<string, string> key = x.first;
            string from = std::get<0>(key);
            string trigger = std::get<1>(key);
            string to = x.second;

            json a = json::array();
            a.push_back(from);
            a.push_back(trigger);
            a.push_back(to);

            j["transfers"].push_back(a);

        }

        return j;
    }

    bool Animation::load(json j)  {
        
        if (!this->TransformableComponent::load(j)) {return false;}
        if (!j.contains("defaultState") || !j["defaultState"].is_string()) {return false;}
        if (!j.contains("currentState") || !j["currentState"].is_string()) {return false;}
        if (!j.contains("states") || !j["states"].is_array()) {return false;}
        if (!j.contains("transfers") || !j["transfers"].is_array()) {return false;}

        if (!j.contains("colour") || !j["colour"].is_array()) {return false;}
        if (j["colour"].size() != 4) {return false;}
        if (!j["colour"][0].is_number()) {return false;}
        if (!j["colour"][1].is_number()) {return false;}
        if (!j["colour"][2].is_number()) {return false;}
        if (!j["colour"][3].is_number()) {return false;}
        
        if (!j.contains("zIndex") || !j["zIndex"].is_number_integer()) {return false;}

        for (auto element : j["states"]) {
            if (element.is_object()) {
                AnimationState* a = AnimationState::load(element);
                if (a != nullptr) {this->addState(a);}
            }
        }

        for (auto element : j["transfers"]) {
            if (element.is_array() && element.size() == 3) {
                if (!element[0].is_string()) {continue;}
                if (!element[1].is_string()) {continue;}
                if (!element[2].is_string()) {continue;}
                this->addTransfer(element[0], element[1], element[2]);
            }
        }

        this->setDefaultState(j["defaultState"]);
        auto search = this->states.find(j["currentState"]);
        if (search != this->states.end()) {
            this->currentState = search->second;
        }

        this->setColour(vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]));
        this->setZIndex(j["zIndex"]);
        
        return true;
    }

    void Animation::setDefaultState(string title)  {

        auto search = this->states.find(title);
        if (search != this->states.end()) {
            this->defaultState = title;
            if (this->currentState == nullptr) {this->currentState = search->second;}
        }
    
    }

    void Animation::setState(string title) {
        auto search = this->states.find(title);
        if (search == this->states.end()) {return;}
        if (this->currentState == search->second) {return;}
        this->currentState = search->second;
        this->currentState->setCurrent(0);
    }

    void Animation::addTransfer(string from, string to, string trigger)  {
        tuple<string, string> key(from, trigger);
        std::pair<tuple<string, string>, string> p(key, to);
        this->transfers.insert(p);
    }

    void Animation::addState(AnimationState* state) {
        std::pair<string, AnimationState*> p(state->getTitle(), state);
        this->states.insert(p);
    }

    void Animation::clearStates() {
        for (auto const& x: this->states) {
            AnimationState* a = x.second;
            delete a;
        }
    }

    void Animation::setColour(vec4 colour) {
        this->colour = colour;
    }

    void Animation::setZIndex(int zIndex) {
        this->zIndex = zIndex;
    }

    vec4 Animation::getColour() {
        return this->colour;
    }

    int Animation::getZIndex() {
        return this->zIndex;
    }

    void Animation::trigger(string trigger)  {
        
        // If we are at the null state, we can't transfer to any other states.
        if (this->currentState == nullptr) {return;}

        // Check if there is a transfer for this trigger on the current state.
        tuple<string, string> key(this->currentState->getTitle(), trigger);
        auto search1 = this->transfers.find(key);
        if (search1 == this->transfers.end()) {return;}
        
        // Find the next state.
        string next = search1->second;
        auto search2 = this->states.find(next);
        if (search2 == this->states.end()) {return;}
        this->currentState = search2->second;
        this->currentState->setCurrent(0);

    }

}
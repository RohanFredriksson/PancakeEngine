#include "pancake/components/animation.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/asset/assetpool.hpp"

#include <utility>
using std::pair;

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

    void AnimationState::addFrame(Sprite* sprite, float duration) {
        AnimationFrame* frame = new AnimationFrame(sprite, duration);
        this->frames.push_back(frame);
    }

    void AnimationState::addFrame(AnimationFrame* frame) {
        this->frames.push_back(frame);
    }

    void AnimationState::addFrames(vector<Sprite*> sprites, float duration) {
        for (Sprite* sprite : sprites) {
            AnimationFrame* frame = new AnimationFrame(sprite, duration);
            this->frames.push_back(frame);
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
        SpriteRenderer* spriterenderer = dynamic_cast<SpriteRenderer*>(this->getEntity()->getComponent("SpriteRenderer"));
        if (spriterenderer == nullptr) {return;}
        spriterenderer->setSprite(this->current->getCurrentSprite());
    }

    json Animation::serialise()  {

        json j = this->Component::serialise();
        j.emplace("defaultState", this->defaultState);
        string currentState = (this->current == nullptr ? "" : this->current->getTitle());
        j.emplace("currentState", currentState);

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
        
        if (!this->Component::load(j)) {return false;}
        if (!j.contains("defaultState") || !j["defaultState"].is_string()) {return false;}
        if (!j.contains("currentState") || !j["currentState"].is_string()) {return false;}
        if (!j.contains("states") || !j["states"].is_array()) {return false;}
        if (!j.contains("transfers") || !j["transfers"].is_array()) {return false;}

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
            this->current = search->second;
        }
        
        return true;
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
        this->current->setCurrent(0);

    }

}
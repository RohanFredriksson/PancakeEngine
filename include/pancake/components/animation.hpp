#pragma once

#include "pancake/core/component.hpp"
#include "pancake/graphics/sprite.hpp"
#include <unordered_map>
#include <tuple>

using std::unordered_map;
using std::tuple;

struct StringTupleHash {
    size_t operator()(const tuple<string, string>& t) const {
        return std::hash<string>()(std::get<0>(t)) ^ std::hash<string>()(std::get<1>(t));
    }
};

struct StringTupleEqual {
    bool operator()(const tuple<string, string>& lhs, const tuple<string, string>& rhs) const {
        return (std::get<0>(lhs) == std::get<0>(rhs)) && (std::get<1>(lhs) == std::get<1>(rhs));
    }
};

struct AnimationFrame {
    Sprite* sprite;
    float duration;
};

class AnimationState {

    private:

        string title;
        vector<AnimationFrame> frames;
        int current;
        float time;
        bool loop;

    public:

        AnimationState();
        void update(float dt);
        //json serialise();
        //AnimationState* load(json j);
        
        void setTitle(string title);
        void setCurrent(int current);
        void setTime(float time);
        void setLoop(bool loop);

        void addFrame(Sprite* sprite, float duration);
        void addFrames(vector<Sprite*> sprites, float duration);

        string getTitle();
        Sprite* getCurrentSprite();
        bool doesLoop();

};

class Animation : public Component {

    private:

        unordered_map<string, AnimationState*> states;
        unordered_map<tuple<string, string>, string, StringTupleHash, StringTupleEqual> transfers;
        AnimationState* current;
        string defaultState;

    public:

        Animation();
        void start() override;
        void end() override;
        void update(float dt) override;
        json serialise() override;
        bool load(json j) override;

        void setDefaultState(string title);
        void addTransfer(string from, string to, string trigger);
        void addState(AnimationState* state);
        void clearStates();

        void trigger(string trigger);

};
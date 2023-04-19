#pragma once

#include "pancake/core/component.hpp"
#include "pancake/graphics/sprite.hpp"

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

        vector<AnimationState*> states;
        AnimationState* current;
        string defaultStateTitle;

    public:

        Animation();
        //void start() override;
        //void update(float dt) override;

        //void trigger(string trigger);


};
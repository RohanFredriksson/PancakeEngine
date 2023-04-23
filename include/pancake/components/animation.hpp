#pragma once

#include "pancake/core/component.hpp"
#include "pancake/graphics/sprite.hpp"
#include <unordered_map>
#include <tuple>
#include <glm/glm.hpp>

using std::unordered_map;
using std::tuple;
using glm::vec4;

namespace Pancake {

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

    class AnimationFrame {

        public:
            
            Sprite* sprite;
            float duration;

            AnimationFrame(Sprite* sprite, float duration);
            json serialise();
            static AnimationFrame* load(json j);

    };

    class AnimationState {

        private:

            string title;
            vector<AnimationFrame*> frames;
            int current;
            float time;
            bool loop;

        public:

            AnimationState();
            ~AnimationState();
            void update(float dt);
            json serialise();
            static AnimationState* load(json j);
            
            void setTitle(string title);
            void setCurrent(int current);
            void setTime(float time);
            void setLoop(bool loop);

            void addFrame(Sprite* sprite, float duration);
            void addFrame(AnimationFrame* frame);
            void addFrames(vector<Sprite*> sprites, float duration);
            void addFrames(vector<AnimationFrame*> frames);

            string getTitle();
            Sprite* getCurrentSprite();
            bool doesLoop();

    };

    class Animation : public TransformableComponent {

        private:

            unordered_map<string, AnimationState*> states;
            unordered_map<tuple<string, string>, string, StringTupleHash, StringTupleEqual> transfers;
            AnimationState* currentState;
            string defaultState;
            int spriterenderer;

            vec4 colour;
            int zIndex;

            vec4 lastColour;
            int lastZIndex;
            vec2 lastPositionOffset;
            vec2 lastSizeScale;
            float lastRotationOffset;
            
            bool dirty;

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

            void setColour(vec4 colour);
            void setZIndex(int zIndex);

            vec4 getColour();
            int getZIndex();

            void trigger(string trigger);

    };

}
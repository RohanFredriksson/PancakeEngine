#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "core/component.hpp"
#include "graphics/primitives/sprite.hpp"

using std::string;
using glm::vec2;
using glm::vec4;

class TextRenderer : public Component {

    private:

        string text;
        //Font* font;
        vec4 colour;
        int zIndex;

        vector<Component*> components;

        string lastText;
        //Font* lastFont;
        vec4 lastColour;
        vec2 lastPosition;
        vec2 lastSize;
        float lastRotation;
        int lastZIndex;
        
        bool dirty;

    public:

        TextRenderer(string text, /*Font* font,*/ vec4 colour, int zIndex);
        ~TextRenderer();

        void update(float dt) override;
        
        // Getters
        string getText();
        //Font* getFont();
        vec4 getColour();
        int getZIndex();
        bool isDirty();
        
        // Setters
        void setText(string text);
        void setColour(vec4 colour);
        void setZIndex(int zIndex);
        void setClean();

};
#pragma once

#include <string>
#include <vector>
#include "pancake/graphics/primitives/sprite.hpp"
#include "pancake/graphics/primitives/texture.hpp"

using std::vector;

class Font {

    private:

        string filename;
        float size;

        int ascent;
        int descent;
        int lineGap;

        vector<int> advances;
        vector<Sprite*> sprites;
        Texture* texture;

    public:

        Font(string filename, float size);
        ~Font();
        
        string getFilename();
        float getSize();
        int getAscent();
        int getDescent();
        int getLineGap();
        int getAdvance(char code);
        Sprite* getSprite(char code);
        Texture* getTexture();

        float getScaleForHeight(float height);

};
#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "pancake/graphics/sprite.hpp"
#include "pancake/graphics/texture.hpp"

using std::vector;
using json = nlohmann::json;

namespace Pancake {

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

            void load(const unsigned char* fontBuffer, float size);

        public:

            Font(string filename, float size);
            Font(float size);
            ~Font();
            json serialise();
            static void load(json j);
            
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

}
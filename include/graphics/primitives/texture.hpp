#pragma once

#include <string>

using std::string;

class Texture {

    private:

        string name;
        unsigned int id;

        void init(string name, unsigned char* image, int width, int height, int channels);

    public:

        Texture(string filename);
        Texture(string name, unsigned char* image, int width, int height, int channels);
        ~Texture();

        void bind();
        void unbind();

};
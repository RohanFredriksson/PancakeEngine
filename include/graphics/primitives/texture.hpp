#pragma once

#include <string>

using std::string;

class Texture {

    private:

        string filename;
        unsigned int id;
        int width;
        int height;
        int channels;

    public:

        Texture(string filename);
        ~Texture();

        void bind();
        void unbind();

};
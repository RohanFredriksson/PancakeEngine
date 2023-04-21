#pragma once

#include <string>
#include <glad/glad.h>

using std::string;

namespace Pancake {

    class Texture {

        private:

            string name;
            unsigned int id;
            int width;
            int height;

            void init(string name, unsigned char* image, int width, int height, int channels);
            void generate(GLint internal, int width, int height, GLenum format, GLenum type);
            void missing();

        public:

            Texture();
            Texture(string filename);
            Texture(string name, unsigned char* image, int width, int height, int channels);
            Texture(GLint internal, int width, int height, GLenum format, GLenum type);
            ~Texture();

            void bind();
            void unbind();
            
            string getName();
            unsigned int getId();
            int getWidth();
            int getHeight();

    };

}
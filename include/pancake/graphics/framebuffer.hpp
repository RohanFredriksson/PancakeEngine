#pragma once

#include <glad/glad.h>
#include "pancake/graphics/texture.hpp"

class Framebuffer {

    private:

        unsigned int fbo;
        Texture* texture;

        void init(GLint internal, int width, int height, GLenum format, GLenum type);

    public:

        Framebuffer(int width, int height);
        Framebuffer(GLint internal, int width, int height, GLenum format, GLenum type);
        ~Framebuffer();

        void bind();
        void unbind();

        Texture* getTexture();

};
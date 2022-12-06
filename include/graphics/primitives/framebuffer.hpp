#pragma once

#include <glad/glad.h>
#include "graphics/primitives/texture.hpp"

class FrameBuffer {

    private:

        unsigned int fbo;
        Texture* texture;

        void init(GLint internal, int width, int height, GLenum format, GLenum type);

    public:

        FrameBuffer(int width, int height);
        FrameBuffer(GLint internal, int width, int height, GLenum format, GLenum type);
        ~FrameBuffer();

        void bind();
        void unbind();

        Texture* getTexture();

};
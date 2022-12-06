#pragma once

#include "graphics/primitives/texture.hpp"

class FrameBuffer {

    private:

        unsigned int fbo;
        Texture* texture;

    public:

        FrameBuffer(int width, int height);
        ~FrameBuffer();

        void bind();
        void unbind();

};
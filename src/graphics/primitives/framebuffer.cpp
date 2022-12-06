#include <stdio.h>
#include <glad/glad.h>
#include "graphics/primitives/framebuffer.hpp"

FrameBuffer::FrameBuffer(int width, int height) {
    
    // Generate the framebuffer.
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    // Initialise the texture to render the data to, and attach it to the framebuffer.
    this->texture = new Texture(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture->getId(), 0);

    // Create a renderbuffer.
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR::FRAMEBUFFER::FRAMEBUFFER_NOT_COMPLETE\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

FrameBuffer::~FrameBuffer() {
    delete this->texture;
}

void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}

void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
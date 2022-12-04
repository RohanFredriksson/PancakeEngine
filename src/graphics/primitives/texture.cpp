#include <stdio.h>
#include <glad/glad.h>
#include "graphics/primitives/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void Texture::init(string name, unsigned char* image, int width, int height, int channels) {
    
    // Store the required variables
    this->name = name;

    if (image != NULL) {

        // Generate the texture on the GPU.
        glGenTextures(1, &this->id);
        glBindTexture(GL_TEXTURE_2D, this->id);

        // Set texture parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (channels == 3) {glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);}
        else if (channels == 4) {glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);}
        else {printf("ERROR::TEXTURE::INVALID_IMAGE_CHANNELS '%d'\n", channels);}

    }

    else {
        printf("ERROR::TEXTURE::IMAGE_LOAD_FAILED '%s'\n", name.c_str());
    }

}

Texture::Texture(string filename) {

    int width;
    int height;
    int channels;

    // Load the image
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    // Initialise the texture.
    this->init(filename, image, width, height, channels);

    // Free the image.
    stbi_image_free(image);

}

Texture::Texture(string name, unsigned char* image, int width, int height, int channels) {
    this->init(name, image, width, height, channels);
}

Texture::~Texture() {
    glDeleteTextures(1, &this->id);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
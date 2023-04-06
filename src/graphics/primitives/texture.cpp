#include <iostream>
#include <glad/glad.h>
#include "pancake/graphics/primitives/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void Texture::init(string name, unsigned char* image, int width, int height, int channels) {
    
    // Store the required variables
    this->name = name;
    this->width = width;
    this->height = height;

    if (image != nullptr && (channels == 3 || channels == 4)) {

        // Generate the texture on the GPU.
        glGenTextures(1, &this->id);
        glBindTexture(GL_TEXTURE_2D, this->id);

        // Set texture parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (channels == 3) {glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);}
        else if (channels == 4) {glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);}

    }

    else {

        // Print a warning message.
        if (!(channels == 3 || channels == 4)) {std::cout << "ERROR::TEXTURE::INVALID_IMAGE_CHANNELS '" << channels << "' '" << name << "'\n";}
        else {std::cout << "ERROR::TEXTURE::IMAGE_LOAD_FAILED '" << name << "'\n";}

        // Generate texture on GPU
        glGenTextures(1, &this->id);
        glBindTexture(GL_TEXTURE_2D, this->id);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Load the image
        unsigned char missing[] = {0,0,0,255,0,255,0,0,255,0,255,0,0};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, missing);

    }

}

void Texture::generate(GLint internal, int width, int height, GLenum format, GLenum type) {

    // Store the string.
    this->name = "generated";

    // Generate texture on GPU
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, type, 0);

}

void Texture::missing() {

    // Label the texture as missing
    this->name = "missing";

    // Generate texture on GPU
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Load the image
    unsigned char image[] = {0,0,0,255,0,255,0,0,255,0,255,0,0};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

}

Texture::Texture() {
    this->missing();
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

Texture::Texture(GLint internal, int width, int height, GLenum format, GLenum type) {
    this->generate(internal, width, height, format, type);
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

string Texture::getName() {
    return this->name;
}

unsigned int Texture::getId() {
    return this->id;
}

int Texture::getWidth() {
    return this->width;
}

int Texture::getHeight() {
    return this->height;
}
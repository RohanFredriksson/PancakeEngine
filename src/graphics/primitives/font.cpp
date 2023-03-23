#include <iostream>
#include "pancake/graphics/primitives/font.hpp"
#include "pancake/util/fonts.hpp"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb/stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

namespace {
    int NUM_CHARACTERS = 128;
}

void Font::load(const unsigned char* fontBuffer, float size) {

    // Prepare the font
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontBuffer, 0)) {
        std::cout << "ERROR::FONT::INIT::FONT_LOADING_FAILED\n";
        throw "ERROR::FONT::INIT::FONT_LOADING_FAILED";
    }

    // Find the scale for a certain pixel height.
    float scale = stbtt_ScaleForPixelHeight(&info, size);

    // Get font's vertical metrics
    stbtt_GetFontVMetrics(&info, &this->ascent, &this->descent, &this->lineGap);
    int ascent = roundf(scale * this->ascent);

    // Determine the total width required to display all characters.
    int width = 0;
    int height;
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        int ax;
	    int lsb;
        stbtt_GetCodepointHMetrics(&info, i, &ax, &lsb);
        width += roundf(ax * scale);
    }

    int average = width / NUM_CHARACTERS;
    int hSpacing = roundf(0.125f * average);

    // Create a square shaped image for the texture.
    int area = width * size * 1.25f;
    width = sqrtf(area) + 1;
    height = ((width / size) + 2) * size;

    // Allocate memory to store the grayscale image.
    unsigned char* mask = (unsigned char*) calloc(width * height, sizeof(unsigned char));

    // Render each character onto the grayscale image.
    int x = 0;
    int line = 0;
    for (int i = 0; i < NUM_CHARACTERS; i++) {

        char character = (char) i;
        char name[] = { character, '\0' };

        int ax;
	    int lsb;
        stbtt_GetCodepointHMetrics(&info, i, &ax, &lsb);
        this->advances.push_back(ax);

        // If the character is going out of the buffer, move to new line
        int advance = roundf(ax * scale);
        if (x + advance + hSpacing >= width) {
            x = 0;
            line++;
        }

        // Get bounding box for characters.
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&info, i, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        // Compute the y value
        int y = (line * size) + ascent + c_y1;
        
        // Create the sprite object.
        float x0 = (float) (x) / (float) width;
        float x1 = (float) (x + advance) / (float) width;
        float y0 = (float) (y - ascent - c_y1 + size) / (float) height;
        float y1 = (float) (y - ascent - c_y1) / (float) height;
        
        vec2 texCoords[4];
        texCoords[0][0] = x1;
        texCoords[0][1] = y1;
        texCoords[1][0] = x1;
        texCoords[1][1] = y0;
        texCoords[2][0] = x0;
        texCoords[2][1] = y0;
        texCoords[3][0] = x0;
        texCoords[3][1] = y1;

        // Initialise the sprite object.
        Sprite* sprite = new Sprite();
        sprite->setName(name);
        sprite->setTexCoords(texCoords);
        this->sprites.push_back(sprite);

        // Render the character
        int byteOffset = x + roundf(lsb * scale) + (y * width);
        stbtt_MakeCodepointBitmap(&info, mask + byteOffset, c_x2 - c_x1, c_y2 - c_y1, width, scale, scale, i);

        // Advance x
        x += advance + hSpacing;

    }

    // Convert the grayscale to a transparent image.
    unsigned char* image = (unsigned char*) calloc(width * height * 4, sizeof(unsigned char));
    for (int i = 0; i < width * height; i++) {
        
        if (mask[i] > 0) {
            image[4 * i + 0] = 255;
            image[4 * i + 1] = 255;
            image[4 * i + 2] = 255;
        } 
        
        else {
            image[4 * i + 0] = 0;
            image[4 * i + 1] = 0;
            image[4 * i + 2] = 0;
        }
        
        image[4 * i + 3] = mask[i];
    }

    // Create the texture
    this->texture = new Texture(filename, image, width, height, 4);

    // Add the texture to each sprite.
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        this->sprites[i]->setTexture(this->texture);
    }

    // Free the allocated memory.
    free(mask);
    free(image);

}

Font::Font(string filename, float size) {
    
    this->filename = filename;
    this->size = size;

    // Read the contents of the file.
    long fileSize;
    unsigned char* fontBuffer;
    FILE* fontFile = fopen(filename.c_str(), "rb");
    if (fontFile == NULL) {
        std::cout << "ERROR::FONT::INIT::FILE_NOT_FOUND\n";
        throw "ERROR::FONT::INIT::FILE_NOT_FOUND";
    }
    fseek(fontFile, 0, SEEK_END);
    fileSize = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    fontBuffer = (unsigned char*) malloc(fileSize);
    fread(fontBuffer, fileSize, 1, fontFile);
    fclose(fontFile);

    // Load the font.
    this->load((const unsigned char*) fontBuffer, this->size);    

    // Debug output
    //stbi_write_png("out.png", width, height, 4, image, width * 4);

    // Free the font buffer.
    free(fontBuffer);
}

Font::Font(float size) {
    this->filename = "default";
    this->size = size;
    const unsigned char* buffer = reinterpret_cast<const unsigned char*>(PIXELLARI);
    this->load(buffer, this->size);
}

Font::~Font() {
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        delete this->sprites[i];
    }
    delete this->texture;
}

string Font::getFilename() {
    return this->filename;
}

float Font::getSize() {
    return this->size;
}

int Font::getAscent() {
    return this->ascent;
}

int Font::getDescent() {
    return this->descent;
}

int Font::getLineGap() {
    return this->lineGap;
}

int Font::getAdvance(char code) {
    int index = (int) code;
    if (index < 0 || index > NUM_CHARACTERS) {return 0;}
    return this->advances[index];
}

Sprite* Font::getSprite(char code) {
    int index = (int) code;
    if (index < 0 || index > NUM_CHARACTERS) {return NULL;}
    return this->sprites[index];
}

Texture* Font::getTexture() {
    return this->texture;
}

float Font::getScaleForHeight(float height) {
    return height / (this->ascent - this->descent);
}
#include <utility>
#include <iostream>
#include <unordered_map>
#include "util/assetpool.hpp"

using std::pair;
using std::unordered_map;

namespace {

    unordered_map<string, Texture*> textures;
    unordered_map<string, Sprite*> sprites;
    unordered_map<string, Font*> fonts;
    unordered_map<string, AudioWave*> audio;

}

void AssetPool::init() {
    TexturePool::init();
    SpritePool::init();
    FontPool::init();
    AudioPool::init();
}

void AssetPool::clear() {
    TexturePool::clear();
    SpritePool::clear();
    FontPool::clear();
    AudioPool::clear();
}

void AssetPool::destroy() {
    TexturePool::destroy();
    SpritePool::destroy();
    FontPool::destroy();
    AudioPool::destroy();
}

void TexturePool::init() {

    // Add the missing texture to the pool.
    Texture* missing = new Texture();
    pair<string, Texture*> p(missing->getName(), missing);
    textures.insert(p);

}

void TexturePool::clear() {
    for (auto const& x : textures) {
        Texture* t = x.second;
        delete t;
    }
    textures.clear();
}

void TexturePool::destroy() {
    TexturePool::clear();
}

Texture* TexturePool::get(string name) {

    // If the texture already exists, return the texture. 
    auto search = textures.find(name);
    if (search != textures.end()) {return search->second;}

    // Attempt to initialise the texture.
    try {
        Texture* texture = new Texture(name);
        pair<string, Texture*> p(texture->getName(), texture);
        textures.insert(p);
        return texture;
    } 
    
    // If the texture could not be initialise return the missing texture.
    catch (...) {
        return TexturePool::get("missing");
    }

}

void SpritePool::init() {

    // Add the empty sprite to the pool
    Sprite* empty = new Sprite("empty", NULL);
    SpritePool::put(empty);

    // Add the missing sprite to the pool
    Sprite* missing = new Sprite("missing", TexturePool::get("missing"));
    SpritePool::put(missing);

}

void SpritePool::clear() {
    for (auto const& x : sprites) {
        Sprite* s = x.second;
        delete s;
    }
    sprites.clear();
}

void SpritePool::destroy() {
    SpritePool::clear();
}

Sprite* SpritePool::get(string name) {
    
    // If the sprite already exists, return the sprite. 
    auto search = sprites.find(name);
    if (search != sprites.end()) {return search->second;}

    std::cout << "ERROR::SPRITEPOOL::GET::SPRITE_NOT_EXIST: '" << name << "'\n";
    return SpritePool::get("missing");

}

bool SpritePool::has(string name) {
    auto search = sprites.find(name);
    return search != sprites.end();
}

void SpritePool::put(Sprite* sprite) {
    pair<string, Sprite*> p(sprite->getName(), sprite);
    sprites.insert(p);
}

void FontPool::init() {

    // Add the default font to the pool.
    Font* font = new Font("assets/fonts/Pixellari.ttf", 64);
    pair<string, Font*> p("default", font);
    fonts.insert(p);

}

void FontPool::clear() {
    for (auto const& x : fonts) {
        Font* f = x.second;
        delete f;
    }
    fonts.clear();
}

void FontPool::destroy() {
    FontPool::clear();
}

Font* FontPool::get(string name) {
    
    // If the font already exists, return the font. 
    auto search = fonts.find(name);
    if (search != fonts.end()) {return search->second;}

    // Attempt to initialise the font.
    try {
        Font* font = new Font(name, 64);
        pair<string, Font*> p(font->getFilename(), font);
        fonts.insert(p);
        return font;
    } 
    
    // If the texture could not be initialise return the missing texture.
    catch (...) {
        return FontPool::get("default");
    }

}

void AudioPool::init() {

}

void AudioPool::clear() {
    for (auto const& x : audio) {
        AudioWave* a = x.second;
        delete a;
    }
    audio.clear();
}

void AudioPool::destroy() {
    AudioPool::clear();
}

AudioWave* AudioPool::get(string name) {
    
    // If the font already exists, return the font. 
    auto search = audio.find(name);
    if (search != audio.end()) {return search->second;}

    // Attempt to initialise the font.
    AudioWave* audioWave = new AudioWave(name);
    pair<string, AudioWave*> p(audioWave->getFilename(), audioWave);
    audio.insert(p);
    return audioWave;

}
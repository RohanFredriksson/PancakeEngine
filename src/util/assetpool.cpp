#include <utility>
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
    return NULL;
}

void SpritePool::init() {

    // Add the empty sprite to the pool
    Sprite* empty = new Sprite("empty", NULL);
    pair<string, Sprite*> p1(empty->getName(), empty);
    sprites.insert(p1);

    // Add the missing sprite to the pool
    Sprite* missing = new Sprite("missing", TexturePool::get("missing"));
    pair<string, Sprite*> p2(missing->getName(), missing);
    sprites.insert(p2);

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
    return NULL;
}

void FontPool::init() {

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

Font* FontPool::get(string name, float size) {
    return NULL;
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
    return NULL;
}
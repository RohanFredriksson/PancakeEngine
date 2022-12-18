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

}

void AssetPool::clear() {

}

void AssetPool::destroy() {

}

void TexturePool::init() {

}

void TexturePool::clear() {

}

void TexturePool::destroy() {

}

Texture* TexturePool::get(string name) {
    return NULL;
}

void SpritePool::init() {

}

void SpritePool::clear() {

}

void SpritePool::destroy() {

}

Sprite* SpritePool::get(string name) {
    return NULL;
}

void FontPool::init() {

}

void FontPool::clear() {

}

void FontPool::destroy() {

}

Font* FontPool::get(string name, float size) {
    return NULL;
}

void AudioWavePool::init() {

}

void AudioWavePool::clear() {

}

void AudioWavePool::destroy() {

}

AudioWave* AudioWavePool::get(string name) {
    return NULL;
}
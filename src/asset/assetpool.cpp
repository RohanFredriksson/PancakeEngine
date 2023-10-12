#include <tuple>
#include <utility>
#include <iostream>
#include <unordered_map>
#include "pancake/asset/assetpool.hpp"
#include "pancake/asset/spritesheet.hpp"

namespace Pancake {

    namespace {

        struct TupleHash {
            size_t operator()(const std::tuple<std::string, float>& t) const {
                return std::hash<std::string>()(std::get<0>(t)) ^ std::hash<float>()(std::get<1>(t));
            }
        };

        struct TupleEqual {
            bool operator()(const std::tuple<std::string, float>& lhs, const std::tuple<std::string, float>& rhs) const {
                return (std::get<0>(lhs) == std::get<0>(rhs)) && (std::get<1>(lhs) == std::get<1>(rhs));
            }
        };

        const float DEFAULT_FONT_SIZE = 64;
        std::unordered_map<std::string, Texture*> textures;
        std::unordered_map<std::string, Sprite*> sprites;
        std::unordered_map<std::tuple<std::string, float>, Font*, TupleHash, TupleEqual> fonts;
        std::unordered_map<std::string, AudioWave*> audio;

    }

    void AssetPool::init() {
        FontPool::init();
        TexturePool::init();
        SpritePool::init();
        AudioPool::init();
    }

    void AssetPool::clear() {
        FontPool::clear();
        TexturePool::clear();
        SpritePool::clear();
        AudioPool::clear();
    }

    void AssetPool::destroy() {
        FontPool::destroy();
        TexturePool::destroy();
        SpritePool::destroy();
        AudioPool::destroy();
    }

    void TexturePool::init() {

        // Add the missing texture to the pool.
        Texture* missing = new Texture();
        std::pair<std::string, Texture*> p("missing", missing);
        textures.insert(p);

    }

    void TexturePool::clear() {
        TexturePool::destroy();
        TexturePool::init();
    }

    void TexturePool::destroy() {
        for (auto const& x : textures) {
            Texture* t = x.second;
            delete t;
        }
        textures.clear();
    }

    Texture* TexturePool::get(std::string name) {

        // If the texture already exists, return the texture. 
        auto search = textures.find(name);
        if (search != textures.end()) {return search->second;}

        // Initialise the texture. If the texture fails to initialise a missing texture will be generated.
        Texture* texture = new Texture(name);
        std::pair<std::string, Texture*> p(texture->getName(), texture);
        textures.insert(p);
        return texture;

    }

    void SpritePool::init() {

        // Add the empty sprite to the pool
        Sprite* empty = new Sprite("empty", nullptr);
        empty->setSerialisable(false);
        SpritePool::put(empty);

        // Add the missing sprite to the pool
        Sprite* missing = new Sprite("missing", TexturePool::get("missing"));
        missing->setSerialisable(false);
        SpritePool::put(missing);

    }

    void SpritePool::clear() {
        SpritePool::destroy();
        SpritePool::init();
    }

    void SpritePool::destroy() {
        Spritesheet::clear();
        for (auto const& x : sprites) {
            Sprite* s = x.second;
            delete s;
        }
        sprites.clear();
    }

    nlohmann::json SpritePool::serialise() {
        nlohmann::json j = nlohmann::json::array();
        for (auto const& x : sprites) {
            Sprite* s = x.second;
            if (s->isSerialisable()) {j.push_back(s->serialise());}
        }
        return j;
    }

    Sprite* SpritePool::get(std::string name) {
        
        // If the sprite already exists, return the sprite. 
        auto search = sprites.find(name);
        if (search != sprites.end()) {return search->second;}

        // Create a sprite with the texture of name.
        Texture* texture = TexturePool::get(name);

        // Create the sprite with the texture. If the texture does not exist, don't serialise the sprite.
        Sprite* sprite = new Sprite(name, texture);
        if (texture->isMissing()) {
            sprite->setSerialisable(false);
            std::cout << "ERROR::SPRITEPOOL::GET::SPRITE_NOT_EXIST: '" << name << "'\n";
        }

        // Add the sprite to the sprite pool.
        SpritePool::put(sprite);
        return sprite;

    }

    bool SpritePool::has(std::string name) {
        auto search = sprites.find(name);
        return search != sprites.end();
    }

    void SpritePool::put(Sprite* sprite) {
        if (sprite == nullptr) {return;}
        std::pair<std::string, Sprite*> p(sprite->getName(), sprite);
        sprites.insert(p);
    }

    void FontPool::init() {

        // Add the default font to the pool.
        Font* d = new Font(DEFAULT_FONT_SIZE);
        Font* p = new Font(DEFAULT_FONT_SIZE);
        std::tuple<std::string, float> dt("default", DEFAULT_FONT_SIZE);
        std::tuple<std::string, float> pt("pixellari", DEFAULT_FONT_SIZE);
        std::pair<std::tuple<std::string, float>, Font*> p1(dt, d);
        std::pair<std::tuple<std::string, float>, Font*> p2(pt, p);
        fonts.insert(p1);
        fonts.insert(p2);

    }

    void FontPool::clear() {
        FontPool::destroy();
        FontPool::init();
    }

    void FontPool::destroy() {
        for (auto const& x : fonts) {
            Font* f = x.second;
            vector<Sprite*> sprites = f->getSprites();
            delete f;
        }
        fonts.clear();
    }

    nlohmann::json FontPool::serialise() {
        nlohmann::json j = nlohmann::json::array();
        for (auto const& x : fonts) {
            Font* f = x.second;
            if (f->getFilename() == "default") {continue;}
            if (f->getFilename() == "pixellari") {continue;}
            j.push_back(f->serialise());
        }
        return j;
    }

    Font* FontPool::get(std::string name) {
        return FontPool::get(name, DEFAULT_FONT_SIZE);
    }

    Font* FontPool::get(std::string name, float size) {

        std::tuple<std::string, float> key(name, size);
        auto search = fonts.find(key);
        if (search != fonts.end()) {return search->second;}

        // Attempt to initialise the font. If the loading fails, the font will be initialised with the default font.
        Font* font = new Font(name, size);
        std::pair<std::tuple<std::string, float>, Font*> p(key, font);
        fonts.insert(p);
        return font;

    }

    bool FontPool::has(std::string name) {
        return FontPool::has(name, DEFAULT_FONT_SIZE);
    }

    bool FontPool::has(std::string name, float size) {
        std::tuple<std::string, float> key(name, size);
        auto search = fonts.find(key);
        return search != fonts.end();
    }

    void AudioPool::init() {

    }

    void AudioPool::clear() {
        AudioPool::destroy();
        AudioPool::init();
    }

    void AudioPool::destroy() {
        for (auto const& x : audio) {
            AudioWave* a = x.second;
            delete a;
        }
        audio.clear();
    }

    nlohmann::json AudioPool::serialise() {
        nlohmann::json j = nlohmann::json::array();
        for (auto const& x : audio) {
            AudioWave* a = x.second;
            j.push_back(a->serialise());
        }
        return j;
    }

    AudioWave* AudioPool::get(std::string name) {
        
        // If the audio already exists, return the audio. 
        auto search = audio.find(name);
        if (search != audio.end()) {return search->second;}

        // Attempt to initialise the audio.
        AudioWave* audioWave = new AudioWave(name);
        std::pair<std::string, AudioWave*> p(audioWave->getFilename(), audioWave);
        audio.insert(p);
        return audioWave;

    }

}
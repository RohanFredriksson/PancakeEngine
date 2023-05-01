#include "pancake/graphics/sprite.hpp"
#include "pancake/asset/assetpool.hpp"
#include <cstring>
#include <imgui.h>

namespace Pancake {

    void Sprite::init(string name, Texture* texture, vec2 texCoords[4]) {
        
        this->name = name;
        this->texture = texture;
        this->serialisable = true;
        
        this->texCoords[0] = texCoords[0];
        this->texCoords[1] = texCoords[1];
        this->texCoords[2] = texCoords[2];
        this->texCoords[3] = texCoords[3];

    }

    Sprite::Sprite() {
        
    }

    Sprite::Sprite(string name, Texture* texture) {

        vec2 texCoords[4];
        texCoords[0] = vec2(1, 1);
        texCoords[1] = vec2(1, 0);
        texCoords[2] = vec2(0, 0);
        texCoords[3] = vec2(0, 1);

        this->init(name, texture, texCoords);
        
    }

    Sprite::Sprite(string name, Texture* texture, vec2 texCoords[4]) {
        this->init(name, texture, texCoords);
    }

    json Sprite::serialise() {
        json j;
        j.emplace("name", this->name);
        j.emplace("texture", this->texture->getName());
        j.emplace("texCoords", json::array());
        for (int i = 0; i < 4; i++) {
            json v = json::array();
            v.push_back(this->texCoords[i].x);
            v.push_back(this->texCoords[i].y);
            j["texCoords"].push_back(v);
        }
        return j;
    }

    void Sprite::load(json j) {

        if (!j.contains("name") || !j["name"].is_string()) {return;}
        if (!j.contains("texture") || !j["texture"].is_string()) {return;}
        if (!j.contains("texCoords") || !j["texCoords"].is_array()) {return;}
        if (j["texCoords"].size() != 4) {return;}

        for (int i = 0; i < 4; i++) {
            if (!j["texCoords"][i].is_array()) {return;}
            if (!j["texCoords"][i][0].is_number()) {return;}
            if (!j["texCoords"][i][1].is_number()) {return;}
        }
        
        // Check if the sprite already exists.
        if (SpritePool::has(j["name"])) {return;}

        // Create the new sprite object.
        string name = j["name"];
        string texName = j["texture"];
        Texture* texture = TexturePool::get(texName);
        vec2 texCoords[4];
        for (int i = 0; i < 4; i++) {
            texCoords[i] = vec2(j["texCoords"][i][0], j["texCoords"][i][1]);
        }

        Sprite* sprite = new Sprite(name, texture, texCoords);
        SpritePool::put(sprite);

    }

    void Sprite::imgui() {

        ImGui::Text("Sprite: ");
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(this->name.c_str())) {

            ImGui::Text("Name: ");
            ImGui::SameLine();
            ImGui::InputText("##SpriteName", (char*) this->name.c_str(), this->name.length(), ImGuiInputTextFlags_ReadOnly);

        }

    }

    string Sprite::getName() {
        return this->name;
    }

    Texture* Sprite::getTexture() {
        return this->texture;
    }

    vec2* Sprite::getTexCoords() {
        return this->texCoords;
    }

    bool Sprite::isSerialisable() {
        return this->serialisable;
    }

    void Sprite::setName(string name) {
        this->name = name;
    }

    void Sprite::setTexture(Texture* texture) {
        this->texture = texture;
    }

    void Sprite::setTexCoords(vec2 texCoords[4]) {
        this->texCoords[0] = texCoords[0];
        this->texCoords[1] = texCoords[1];
        this->texCoords[2] = texCoords[2];
        this->texCoords[3] = texCoords[3];
    }

    void Sprite::setSerialisable(bool serialisable) {
        this->serialisable = serialisable;
    }

}
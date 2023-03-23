#pragma once

#include <deque>
#include <vector>
#include "pancake/graphics/primitives/texture.hpp"
#include "pancake/graphics/primitives/shader.hpp"
#include "pancake/graphics/renderer/spriterenderer.hpp"

using std::deque;
using std::vector;

class RenderBatch;

class Renderer {

    private:

        deque<RenderBatch*> batches;

    public:

        Renderer();
        ~Renderer();

        void render();
        void add(SpriteRenderer* sprite);
        void remove(SpriteRenderer* sprite);
        static void bindShader(Shader* shader);
        static Shader* getBoundShader(Shader* shader);

};

class RenderBatch {

    private:

        Renderer* renderer;
        vector<SpriteRenderer*> sprites;
        vector<Texture*> textures;
        
        float* vertices;
        unsigned int vao;
        unsigned int vbo;
        int zIndex;

    public:

        RenderBatch(Renderer* renderer, int zIndex);
        ~RenderBatch();

        static void generateIndices(int* elements);
        static void loadElementIndices(int* elements, int index);
        void loadVertexProperties(int index);

        void render();
        void addSprite(SpriteRenderer* sprite);
        void removeSprite(SpriteRenderer* sprite);

        void addTexture(Texture* texture);
        void removeTextureIfNotUsed(Texture* texture);

        bool hasRoom();
        bool hasTextureRoom();
        bool hasTexture(Texture* texture);

        int getZIndex();

};
#pragma once

#include <vector>
#include "graphics/primitives/texture.hpp"
#include "graphics/primitives/shader.hpp"
#include "graphics/renderer/spriterenderer.hpp"

using std::vector;

class RenderBatch;

class Renderer {

    private:

        vector<RenderBatch*> batches;

    public:

        Renderer();
        ~Renderer();

        void render();
        void add(SpriteRenderer* sprite);
        void remove(SpriteRenderer* sprite);
        void bindShader(Shader* shader);
        Shader* getBoundShader(Shader* shader);

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

};
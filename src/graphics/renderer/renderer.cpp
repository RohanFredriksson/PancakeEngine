#include "graphics/renderer/renderer.hpp"
#include <stdio.h>

namespace {

    const int POS_SIZE = 2;
    const int COLOUR_SIZE = 4;
    const int TEX_COORDS_SIZE = 2;
    const int TEX_ID_SIZE = 1;
    const int ENTITY_ID_SIZE = 1;

    const int POS_OFFSET = 0;
    const int COLOUR_OFFSET = POS_OFFSET + POS_SIZE * sizeof(float);
    const int TEX_COORDS_OFFSET = COLOUR_OFFSET + COLOUR_SIZE * sizeof(float);
    const int TEX_ID_OFFSET = TEX_COORDS_OFFSET + TEX_COORDS_SIZE * sizeof(float);
    const int ENTITY_ID_OFFSET = TEX_ID_OFFSET + TEX_ID_SIZE * sizeof(float);

    const int VERTEX_SIZE = POS_SIZE + COLOUR_SIZE + TEX_COORDS_SIZE + TEX_ID_SIZE + ENTITY_ID_SIZE;
    const int VERTEX_SIZE_BYTES = VERTEX_SIZE * sizeof(float);

    const int TEXTURES_SIZE = 7;
    const int MAX_BATCH_SIZE = 1000;

}

Renderer::Renderer() {
    
}

Renderer::~Renderer() {

}

void Renderer::render() {

}

void Renderer::add(SpriteRenderer* sprite) {

}

void Renderer::remove(SpriteRenderer* sprite) {

}

void Renderer::bindShader(Shader* shader) {

}

Shader* Renderer::getBoundShader(Shader* shader) {
    return NULL;
}

RenderBatch::RenderBatch(Renderer* renderer, int zIndex) {

}

void RenderBatch::generateIndices(int* elements) {

}

void RenderBatch::loadVertexProperties(int index) {

}

void RenderBatch::render() {

}

void RenderBatch::addSprite(SpriteRenderer* sprite) {

}

void RenderBatch::removeSprite(SpriteRenderer* sprite) {

}

void RenderBatch::addTexture(Texture* texture) {

}

void RenderBatch::removeTexture(Texture* texture) {

}

void RenderBatch::removeTextureIfNotUsed(Texture* texture) {

}

bool RenderBatch::hasRoom() {
    return false;
}

bool RenderBatch::hasTextureRoom() {
    return false;
}

bool RenderBatch::hasTexture(Texture* texture) {
    return false;
}
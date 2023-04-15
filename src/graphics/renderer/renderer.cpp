#include <cstdlib>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "pancake/graphics/renderer/renderer.hpp"
#include "pancake/core/window.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

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

    const int MAX_TEXTURES_SIZE = 7;
    const int MAX_BATCH_SIZE = 1000;

    Shader* boundShader = nullptr;

}

Renderer::Renderer() {
    
}

Renderer::~Renderer() {
    for (RenderBatch* current : this->batches) {
        delete current;
    }
}

void Renderer::render() {
    for (RenderBatch* current : this->batches) {
        current->render();
    }
}

void Renderer::add(SpriteRenderer* sprite) {

    // Try and put the sprite in any of the preexisting batches.
    for (RenderBatch* current : this->batches) {
        if (current->hasRoom() && current->getZIndex() == sprite->getZIndex()) {
            if (sprite->getSprite()->getTexture() == nullptr || current->hasTexture(sprite->getSprite()->getTexture()) || current->hasTextureRoom()) {
                current->addSprite(sprite);
                return;
            }
        }
    }

    // Create a new render batch.
    RenderBatch* batch = new RenderBatch(this, sprite->getZIndex());
    batch->addSprite(sprite);

    // Insert to maintain sortedness.
    if (this->batches.size() == 0) {this->batches.push_back(batch);}
    else if (this->batches[0]->getZIndex() >= sprite->getZIndex()) {this->batches.push_front(batch);}
    else if (this->batches[this->batches.size()-1]->getZIndex() <= sprite->getZIndex()) {this->batches.push_back(batch);}
    else {

        // This can be sped up with binary search.
        for (int i = 1; i < this->batches.size(); i++) {

            RenderBatch* previous = this->batches[i-1];
            RenderBatch* current = this->batches[i];

            if (previous->getZIndex() < sprite->getZIndex() && current->getZIndex() >= sprite->getZIndex()) {
                this->batches.insert(this->batches.begin() + i, batch);
                return;
            }

        }

    }

}

void Renderer::remove(SpriteRenderer* sprite) {
    for (RenderBatch* current : this->batches) {
        current->removeSprite(sprite);
    }
}

void Renderer::bindShader(Shader* shader) {
    boundShader = shader;
}

Shader* Renderer::getBoundShader(Shader* shader) {
    return boundShader;
}

RenderBatch::RenderBatch(Renderer* renderer, int zIndex) {

    this->renderer = renderer;
    this->vertices = (float*) malloc(MAX_BATCH_SIZE * 4 * VERTEX_SIZE_BYTES);
    this->zIndex = zIndex;

    // Generate and bind a Vertex Array Object
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    
    // Allocate space for vertices
    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE * 4 * VERTEX_SIZE_BYTES, nullptr, GL_DYNAMIC_DRAW);

    // Create and upload the indices buffer.
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    int* indices = (int*) malloc(MAX_BATCH_SIZE * 6 * sizeof(int));
    RenderBatch::generateIndices(indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_BATCH_SIZE * 6 * sizeof(int), indices, GL_STATIC_DRAW);
    free(indices);

    // Enable the buffer attribute pointers
    glVertexAttribPointer(0, POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void*) POS_OFFSET);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, COLOUR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void*) COLOUR_OFFSET);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, TEX_COORDS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void*) TEX_COORDS_OFFSET);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, TEX_ID_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void*) TEX_ID_OFFSET);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, ENTITY_ID_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void*) ENTITY_ID_OFFSET);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

RenderBatch::~RenderBatch() {
    free(this->vertices);
}

void RenderBatch::generateIndices(int* elements) {

    for (int i = 0; i < MAX_BATCH_SIZE; i++) {
        RenderBatch::loadElementIndices(elements, i);
    }

}

void RenderBatch::loadElementIndices(int* elements, int index) {

    int offsetArrayIndex = 6 * index;
    int offset = 4 * index;

    // Triangle 1
    elements[offsetArrayIndex + 0] = offset + 3;
    elements[offsetArrayIndex + 1] = offset + 2;
    elements[offsetArrayIndex + 2] = offset + 0;

    // Triangle 2
    elements[offsetArrayIndex + 3] = offset + 0;
    elements[offsetArrayIndex + 4] = offset + 2;
    elements[offsetArrayIndex + 5] = offset + 1;

}

void RenderBatch::loadVertexProperties(int index) {

    SpriteRenderer* sprite = this->sprites[index];

    // Find the offset with array (4 vertices per sprite).
    int offset = index * 4 * VERTEX_SIZE;

    vec4 colour = sprite->getColour();
    vec2* texCoords = sprite->getSprite()->getTexCoords();

    int texId = 0;
    if (sprite->getSprite()->getTexture() != nullptr) {
        for (int i = 0; i < this->textures.size(); i++) {
            if (this->textures[i] == sprite->getSprite()->getTexture()) {
                texId = i + 1;
                break;
            }
        }
    }

    // Get the transform for the sprite renderer.
    vec2 position = sprite->getPosition();
    vec2 size = sprite->getSize();
    float rotation = sprite->getRotation();

    bool isRotated = rotation != 0.0f;
    mat4 transformMatrix = mat4(1.0f);

    if (isRotated) {
        //vec3 translateVector = vec3(position.x, position.y, 0.0f); // FOR SOME REASON THIS DOESNT WORK
        //transformMatrix = glm::translate(transformMatrix, translateVector);
        vec3 scaleVector = vec3(size.x, size.y, 1.0f);
        transformMatrix = glm::scale(transformMatrix, scaleVector);
        vec3 rotateVector = vec3(0.0f, 0.0f, 1.0f);
        transformMatrix = glm::rotate(transformMatrix, -rotation, rotateVector);
    }

    // Add vertices with the appropriate properties.
    float xAdd = 0.5f;
    float yAdd = 0.5f;

    for (int i = 0; i < 4; i++) {

        if (i == 1) {yAdd = -0.5f;}
        else if (i == 2) {xAdd = -0.5f;}
        else if (i == 3) {yAdd = 0.5f;}

        vec4 currentPos = vec4(
            position.x + (xAdd * size.x),
            position.y + (yAdd * size.y),
            0.0f, 1.0f
        );

        if (isRotated) {
            vec4 posVector = vec4(xAdd, yAdd, 0.0f, 1.0f);
            currentPos = posVector * transformMatrix;
            currentPos.x += position.x; // TEMPORARY SOLUTION
            currentPos.y += position.y; // TEMPORARY SOLUTION
        }

        // Load Position
        this->vertices[offset + 0] = currentPos.x;
        this->vertices[offset + 1] = currentPos.y;

        // Load Colour
        this->vertices[offset + 2] = colour.x;
        this->vertices[offset + 3] = colour.y;
        this->vertices[offset + 4] = colour.z;
        this->vertices[offset + 5] = colour.w;

        // Load Texture Coordinates
        this->vertices[offset + 6] = texCoords[i].x;
        this->vertices[offset + 7] = texCoords[i].y;

        // Load Texture ID
        this->vertices[offset + 8] = texId;

        // Load Entity ID
        this->vertices[offset + 9] = (float) sprite->getEntity()->getId();

        offset += VERTEX_SIZE;

    }

}

void RenderBatch::render() {

    bool rebuffer = false;
    for (int i = 0; i < this->sprites.size(); i++) {

        SpriteRenderer* current = this->sprites[i];

        if (current->isDirty()) {

            if ((!this->hasTexture(current->getSprite()->getTexture()) && current->getSprite()->getTexture() != nullptr) || (current->getZIndex() != this->zIndex)) {
                this->removeSprite(current);
                this->renderer->add(current);
                i--;
            }

            else {
                this->loadVertexProperties(i);
                current->setClean();
                rebuffer = true;
            }

        }

    }

    if (rebuffer) {
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_BATCH_SIZE * 4 * VERTEX_SIZE_BYTES, this->vertices);
    }

    // Use shader
    boundShader->bind();

    Camera* camera = Window::getScene()->getCamera();

    mat4 view = camera->getView();
    mat4 projection = camera->getProjection();
    boundShader->uploadMat4("uProjection", projection);
    boundShader->uploadMat4("uView", view);

    for (int i = 0; i < this->textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        this->textures[i]->bind();
    }
    int slots[] = {0, 1, 2, 3, 4, 5, 6, 7};
    boundShader->uploadIntArray("uTextures", MAX_TEXTURES_SIZE+1, slots);

    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->sprites.size() * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for (int i = 0; i < this->textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        this->textures[i]->unbind();
    }
    boundShader->unbind();

}

void RenderBatch::addSprite(SpriteRenderer* sprite) {
    
    // If full do not attempt to add.
    if (!this->hasRoom()) {return;}

    // Get the index and add it to the list.
    int index = this->sprites.size();
    this->sprites.push_back(sprite);

    // Add the sprite's texture, if the batch does not have it.
    if (sprite->getSprite()->getTexture() != nullptr) {
        this->addTexture(sprite->getSprite()->getTexture());
    }

    // Add properties to local vertices array.
    this->loadVertexProperties(index);

}

void RenderBatch::removeSprite(SpriteRenderer* sprite) {

    // Remove the sprite from the batch if in the batch.
    bool removed = false;
    int n = this->sprites.size();
    for (int i = 0; i < n; i++) {
        SpriteRenderer* current = this->sprites[i];
        if (current == sprite) {
            this->sprites.erase(this->sprites.begin() + i);
            removed = true;
            break;
        }
    }

    // If removed a sprite, remove the texture if it is not being used by another sprite.
    if (removed) {
        this->removeTextureIfNotUsed(sprite->getSprite()->getTexture());
    }

}

void RenderBatch::addTexture(Texture* texture) {
    if (this->hasTexture(texture)) {return;}
    if (this->hasTextureRoom()) {this->textures.push_back(texture);}
}

void RenderBatch::removeTextureIfNotUsed(Texture* texture) {

    // If the texture is nullptr, we don't need to remove it.
    if (texture == nullptr) {return;}

    // Check if any sprite in the batch uses the texture,
    // If no sprite does, we can remove it.
    for (SpriteRenderer* current : this->sprites) {
        if (current->getSprite()->getTexture() == texture) {return;}
    }

    // Remove the texture.
    int n = this->textures.size();
    for (int i = 0; i < n; i++) {
        Texture* current = this->textures[i];
        if (current == texture) {
            this->textures.erase(this->textures.begin() + i);
            return;
        }
    }

}

bool RenderBatch::hasRoom() {
    return this->sprites.size() < MAX_BATCH_SIZE;
}

bool RenderBatch::hasTextureRoom() {
    return this->textures.size() < MAX_TEXTURES_SIZE;
}

bool RenderBatch::hasTexture(Texture* texture) {
    for (Texture* current : this->textures) {
        if (current == texture) {return true;}
    }
    return false;
}

int RenderBatch::getZIndex() {
    return this->zIndex;
}
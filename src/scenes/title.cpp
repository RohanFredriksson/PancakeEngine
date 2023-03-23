#include "pancake/scenes/title.hpp"
#include "pancake/physics/primitives/box.hpp"
#include "pancake/physics/primitives/circle.hpp"
#include "pancake/graphics/primitives/font.hpp"
#include "pancake/graphics/renderer/textrenderer.hpp"
#include "pancake/util/assetpool.hpp"

using glm::vec2;
using glm::vec4;

void TitleInit(Scene* scene) {

    Entity* entity;
    Texture* texture;
    Sprite* sprite;
    SpriteRenderer* spriterenderer;
    Box* box;
    Circle* circle;
    Rigidbody* rigidbody;
    TextRenderer* textrenderer;

    // BOX 1
    entity = new Entity(vec2(-4.5f, 5.0f), vec2(1.0f, 1.0f), 0.0f);

    texture = TexturePool::get("assets/textures/armaan.png");
    sprite = new Sprite("armaan", texture);
    spriterenderer = new SpriteRenderer();
    spriterenderer->setSprite(sprite);

    rigidbody = new Rigidbody();
    rigidbody->setRestitution(0.2f);
    rigidbody->setFriction(0.0f);
    rigidbody->setVelocity(vec2(1.0f, 0.0f));
    rigidbody->setFixedOrientation(true);

    box = new Box();
    box->setMass(1.0f);
    box->setSize(vec2(1.0f, 1.0f));
    rigidbody->addCollider(box);

    entity->addComponent(spriterenderer);
    entity->addComponent(rigidbody);
    scene->addEntity(entity);

    // BOX 2
    entity = new Entity(vec2(0.0f, 0.0f), vec2(11.0f, 1.0f), 0.0f);

    texture = TexturePool::get("assets/textures/ainsley.png");
    sprite = new Sprite("ainsley", texture);
    spriterenderer = new SpriteRenderer();
    spriterenderer->setSprite(sprite);
    
    rigidbody = new Rigidbody();
    rigidbody->setRestitution(1.0f);
    rigidbody->setFriction(0.0f);
    
    box = new Box();
    box->setMass(0.0f);
    box->setSize(vec2(11.0f, 1.0f));
    rigidbody->addCollider(box);

    entity->addComponent(spriterenderer);
    entity->addComponent(rigidbody);
    scene->addEntity(entity);

}
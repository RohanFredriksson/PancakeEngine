#include <string>
#include <vector>
#include <deque>
#include <glm/glm.hpp>
#include "core/scene.hpp"

#include "physics/primitives/box.hpp"
#include "physics/primitives/circle.hpp"
#include "graphics/primitives/font.hpp"
#include "graphics/renderer/textrenderer.hpp"

#include "util/assetpool.hpp"

using std::deque;
using std::pair;
using glm::vec2;

using glm::vec4;

Scene::Scene(string name) {

    this->name = name;
    this->camera = new Camera(vec2(0.0f, 0.0f), vec2(12.0f, 12.0f), 1.0f);
    this->renderer = new Renderer();
    this->physics = new World(1.0f / 60.0f, vec2(0.0f, -10.0f));

    Entity* entity;
    Texture* texture;
    Sprite* sprite;
    SpriteRenderer* spriterenderer;
    Box* box;
    Circle* circle;
    Rigidbody* rigidbody;
    TextRenderer* textrenderer;

    entity = new Entity(vec2(0.0f, 0.0f), vec2(2.0f, 2.0f), 0.0f);

    texture = TexturePool::get("assets/textures/armaan.png");
    sprite = new Sprite("armaan", texture);
    spriterenderer = new SpriteRenderer();
    spriterenderer->setSprite(sprite);

    rigidbody = new Rigidbody();
    circle = new Circle();
    
    rigidbody->setCollider(circle);
    rigidbody->setMass(0.0f);
    rigidbody->setCor(1.0f);
    circle->setRadius(1.0f);

    entity->addComponent(spriterenderer);
    entity->addComponent(rigidbody);
    this->addEntity(entity);

    entity = new Entity(vec2(-2.5f, 0.0f), vec2(1.0f, 1.0f), 0.0f);

    texture = TexturePool::get("assets/textures/ainsley.png");
    sprite = new Sprite("ainsley", texture);
    spriterenderer = new SpriteRenderer();
    spriterenderer->setSprite(sprite);
    
    rigidbody = new Rigidbody();
    rigidbody->setVelocity(vec2(1.0f, 10.0f));
    rigidbody->setMass(1.0f);
    rigidbody->setCor(0.75f);
    //rigidbody->setFixedOrientation(true);
    rigidbody->setAngularVelocity(-35.0f);

    /*
    circle = new Circle();
    circle->setPositionOffset(vec2(0.5f, 0.0f));
    rigidbody->addCollider(circle);

    circle = new Circle();
    circle->setPositionOffset(vec2(-0.5f, 0.0f));
    rigidbody->addCollider(circle);
    */
    
    box = new Box();
    box->setSize(vec2(2.0f, 1.0f));
    rigidbody->addCollider(box);

    entity->addComponent(spriterenderer);
    entity->addComponent(rigidbody);
    this->addEntity(entity);

}

Scene::~Scene() {
    
    // Delete all scene elements.
    delete this->camera;
    delete this->renderer;
    delete this->physics;

    // Delete all entities and their components.
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        delete e;
    }

}

void Scene::addNewComponents() {

    // For each entity, check their new component list.
    // If there are components in this list, 
    // try and add them to the renderer and physics system.
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        for (Component* c : e->getNewComponents()) {
            
            // If the component is of the correct subclass, add it to its specific system.
            if (dynamic_cast<SpriteRenderer*>(c) != nullptr) {this->renderer->add((SpriteRenderer*) c);}
            if (dynamic_cast<Rigidbody*>(c) != nullptr) {this->physics->add((Rigidbody*) c);}

            // Add the component to the map for quick lookups.
            this->components[c->getId()] = c;
        }
        e->clearNewComponents();
    }
    
}

void Scene::removeDeadComponents() {

    // For each entity, check their dead component list.
    // If there are components in this list, delete them.
    for (auto const& x : this->entities) {
        Entity* e = x.second;
        for (int id : e->getDeadComponentIds()) {
            this->components.erase(id);
        }
        e->clearNewComponents();
    }

}

void Scene::update(float dt) {

    // Add all new components to their specific systems.
    this->addNewComponents(); 

    // Adjust the projection and step the physics engine.
    this->camera->adjustProjection();
    this->physics->update(dt);

    // Update all the entities.
    deque<int> deadIds;
    for (auto const& x : this->entities) {
        Entity* e = x.second;

        // If the component is not dead, update it.
        if (!e->isDead()) {
            e->update(dt);
        } 
        
        // If the component is dead, add its index a clear list.
        else {
            deadIds.push_front(e->getId());
        }

    }

    // Delete all dead elements
    for (int i : deadIds) {
        
        // Find the entity using its id.
        auto search = this->entities.find(i);
        Entity* e = search->second;

        // Remove all components associated with the entity from the map.
        for (Component* c : e->getComponents()) {
            this->components.erase(c->getId());
        }

        // Delete the entity.
        delete e;
        this->entities.erase(i);

    }
    deadIds.clear();

    // Add all new components to their specific systems.
    this->addNewComponents(); 
    this->removeDeadComponents();

}

void Scene::render() {
    this->renderer->render();
    this->physics->render();
}

Camera* Scene::getCamera() {
    return this->camera;
}

Renderer* Scene::getRenderer() {
    return this->renderer;
}

World* Scene::getPhysics() {
    return this->physics;
}

void Scene::addEntity(Entity* entity) {
    pair<int, Entity*> p(entity->getId(), entity);
    this->entities.insert(p);
}

Entity* Scene::getEntityById(int id) {
    auto search = this->entities.find(id);
    if (search == this->entities.end()) {return NULL;}
    Entity* e = search->second;
    return e;
}

Component* Scene::getComponentById(int id) {
    auto search = this->components.find(id);
    if (search == this->components.end()) {return NULL;}
    Component* c = search->second;
    return c;
}
#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <glm/glm.hpp>
#include "core/scene.hpp"

#include "physics/primitives/box.hpp"
#include "physics/primitives/circle.hpp"

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

    entity = new Entity(vec2(0.0f, 0.0f), vec2(5.0f, 1.0f), 0.0f);

    texture = new Texture("assets/textures/armaan.png");
    sprite = new Sprite("armaan", texture);
    spriterenderer = new SpriteRenderer(sprite, vec4(1.0f, 1.0f, 1.0f, 1.0f), 0);

    rigidbody = new Rigidbody();
    box = new Box(vec2(5.0f, 1.0f));

    rigidbody->setCollider(box);
    rigidbody->setMass(0.0f);
    rigidbody->setCor(1.0f);

    box->setRigidbody(rigidbody);

    entity->addComponent(spriterenderer);
    entity->addComponent(rigidbody);
    entity->addComponent(box);
    this->addEntity(entity);

    entity = new Entity(vec2(-4.0f, 0.0f), vec2(1.0f, 1.0f), 0.0f);

    texture = new Texture("assets/textures/ainsley.png");
    sprite = new Sprite("ainsley", texture);
    spriterenderer = new SpriteRenderer(sprite, vec4(1.0f, 1.0f, 1.0f, 1.0f), 0);

    rigidbody = new Rigidbody();
    circle = new Circle(0.5f);
    
    rigidbody->setCollider(circle);
    rigidbody->setVelocity(vec2(1.0f, 10.0f));
    rigidbody->setMass(1.0f);
    rigidbody->setCor(0.75f);

    circle->setRigidbody(rigidbody);

    entity->addComponent(spriterenderer);
    entity->addComponent(rigidbody);
    entity->addComponent(circle);
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
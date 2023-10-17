# Pancake Game Engine

## Description
This is a lightweight 2D game engine written in C++. The engine is designed to be easy to use and highly customizable. It provides a set of basic functionalities to get you started with creating a 2D game. The engine includes a graphics renderer, audio manager, input manager and entity-component system.

## Dependencies
The engine requires the following dependencies:
- [glfw](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [glad](https://glad.dav1d.de/)
- [stb](https://github.com/nothings/stb)
- [json](https://github.com/nlohmann/json)
- [imgui](https://github.com/ocornut/imgui)
- [soloud](https://github.com/jarikomppa/soloud)

## Getting Started
To use the engine in your project, follow these steps:

1. Clone or download the project template from [here](https://github.com/RohanFredriksson/PancakeTemplate).
2. In the root directory of the project run the following command to download all submodules.
```
git submodule update --init --recursive
```
3. Construct your own game components by creating new classes inheriting from the Component Class.
4. Construct a Scene Initialiser Method which will be executed on game start. 
5. In the src/main file in the template, update the template to run the game.
6. To build the game, in the **build** directory in the template run the following commands.
```
cmake ..
make
```

## Example
```c++
#include "pancake/pancake.hpp"
using namespace Pancake;

class MousePanHandler : public Component {
   
    public:

        MousePanHandler() : Component("MousePanHandler") {

        }

        void update(float dt) override {
            
            if (MouseListener::isMouseDragging()) {
                if (MouseListener::getDx() != 0) {Window::getScene()->getCamera()->addPosition(vec2(-MouseListener::getWorldDx(), 0.0f));}
                if (MouseListener::getDy() != 0) {Window::getScene()->getCamera()->addPosition(vec2(0.0f, -MouseListener::getWorldDy()));}
            }

        } 
        
};

REGISTER(Component, MousePanHandler);

void TestInit(Scene* scene) {

    Entity* entity;
    MousePanHandler* mousepanhandler;
    Rigidbody* rigidbody;
    Circle* circle;
    Box* box;

    // Set the scene name
    scene->setName("Test Scene");

    // Mouse Movement Handler
    entity = new Entity();
    mousepanhandler = new MousePanHandler();
    entity->addComponent(mousepanhandler);
    scene->addEntity(entity);

    // Ground
    entity = new Entity(vec2(0.0f, -5.0f), vec2(1.0f, 1.0f), 0.0f);
    rigidbody = new Rigidbody();
    box = new Box();
    box->setSize(vec2(10.0f, 1.0f));
    rigidbody->addCollider(box);
    entity->addComponent(rigidbody);
    scene->addEntity(entity);

    // Falling Box
    entity = new Entity(vec2(-1.0f, 2.0f), vec2(1.0f, 1.0f), 0.0f);
    rigidbody = new Rigidbody();
    box = new Box();
    box->setMass(1.0f);
    box->setSize(vec2(0.5f, 1.0f));
    box->setRotationOffset(0.5f);
    rigidbody->addCollider(box);
    rigidbody->setFriction(0.15f);
    rigidbody->setRestitution(0.3f);
    entity->addComponent(rigidbody);
    scene->addEntity(entity);

}

int main(int argc, char* argv[]) {
    Pancake::load(TestInit);
    Pancake::start();
}
```

## Features
The engine provides the following features:
- 2D graphics rendering using OpenGL.
- 2D rigidbody physics engine supporting arbitrary collider shapes.
- Audio playback and management using SoLoud.
- Input management for keyboard, mouse and gamepad.
- Entity-component system for managing game objects.
- Resource loading from file system.
- JSON serialization and deserialization for game data.
- Debug drawing for collision shapes and other debugging purposes.

## Contributions
Contributions are welcome! If you find any bugs or have suggestions for improvements, feel free to open an issue or submit a pull request on GitHub.

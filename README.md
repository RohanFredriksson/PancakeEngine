# Pancake Game Engine

## Description
This is a lightweight 2D game engine written in C++. The engine is designed to be easy to use and highly customizable. It provides a set of basic functionalities to get you started with creating a 2D game. The engine includes a graphics renderer, audio manager, input manager and entity-component system.

## Example
<img src="https://github.com/RohanFredriksson/PancakeEngine/blob/master/example.gif" alt="Demo" width="50%">

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

On Linux, GLFW and SoLoud need the X11, OpenGL and ALSA development packages:
```
sudo apt-get install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev libasound2-dev
```

## Building the Engine on its Own
The engine can also be built directly, which builds its tests and a demo sandbox.
```
git submodule update --init --recursive
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/examples/pancake_demo
```

| Option | Default | Description |
| --- | --- | --- |
| `PANCAKE_BUILD_TESTS` | On when top level | Build the unit and engine tests. |
| `PANCAKE_BUILD_EXAMPLES` | On when top level | Build the demo sandbox in `examples/`. |
| `PANCAKE_ASAN` | Off | Build the engine, and everything linking it, with AddressSanitizer. |

The engine tests open a real window. On a machine without a display, they run inside `xvfb-run` when it is installed.

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

// This line allows the MousePanHandler component to be loaded from a save file if necessary.
REGISTER(Component, MousePanHandler);

void TestInit(Scene* scene) {

    Entity* entity;
    MousePanHandler* mousepanhandler;
    Rigidbody* rigidbody;
    BoxCollider* box;

    // Set the scene name
    scene->setName("Test Scene");

    // Mouse Movement Handler
    entity = new Entity();
    mousepanhandler = new MousePanHandler();
    entity->addComponent(mousepanhandler);
    scene->addEntity(entity);

    // Gravity applies to every body by default. It can be changed per scene, or per body with setGravityScale.
    scene->getPhysics()->setGravity(vec2(0.0f, -10.0f));

    // Ground. A body with no mass is static.
    entity = new Entity(vec2(0.0f, -5.0f), vec2(1.0f, 1.0f), 0.0f);
    rigidbody = new Rigidbody();
    box = new BoxCollider();
    box->setSize(vec2(10.0f, 1.0f));
    rigidbody->addCollider(box);
    entity->addComponent(rigidbody);
    scene->addEntity(entity);

    // Falling BoxCollider
    entity = new Entity(vec2(-1.0f, 2.0f), vec2(1.0f, 1.0f), 0.0f);
    rigidbody = new Rigidbody();
    box = new BoxCollider();
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
- 2D rigidbody physics with box and circle colliders, combinable into compound bodies, solved with warm started sequential impulses so stacks stay stable.
- World gravity, and force generator components for custom forces that act only on the bodies that opt in.
- Collision listeners, sensors and raycasts.
- Spatial queries to find entities in an area, near a point or under the mouse.
- Audio playback and management using SoLoud.
- Input management for keyboard and mouse, with pressed, held and released events.
- Entity-component system for managing game objects, with typed component access and named entities.
- Resource loading from file system.
- JSON serialization and deserialization for game data, including custom components.
- An in-game console with built-in commands (`help`, `save`, `load`, `reset`, `debug`, `gravity`, `entities`, `inspector`) and support for your own.
- An entity inspector: click an entity to view and edit its components.
- Debug drawing for collision shapes and other debugging purposes.

## Working with Entities
```c++
Entity* player = new Entity(vec2(0.0f, 0.0f));
player->setName("Player");                        // Saved with the scene.
Rigidbody* body = player->addComponent<Rigidbody>();
scene->addEntity(player);

Entity* found = scene->findEntity("Player");      // Or findEntities for every match.
Rigidbody* r = found->getComponent<Rigidbody>();  // nullptr if it has none. Subclasses match too.
for (Entity* e : scene->getEntitiesWith<Rigidbody>()) {}

if (KeyListener::isKeyBeginDown(GLFW_KEY_SPACE)) {} // Pressed this frame.
if (KeyListener::isKeyEndDown(GLFW_KEY_SPACE)) {}   // Released this frame.

// load accepts any callable, including capturing lambdas.
int level = 2;
Pancake::load([level](Scene* scene) { /* build the level */ });
```

## Debugging Tools
```c++
Pancake::console(true);    // Open the console. Pancake::console() toggles it.
Pancake::inspector(true);  // Open the inspector. Pancake::inspector() toggles it.
Pancake::debug(true);      // Draw physics colliders, bounds and contacts.

// Add a console command.
Console::registerCommand("spawn", "spawn <x> <y>: Spawn a crate.", [](std::vector<std::string> args) {
    // ...
});
```

## Upgrading
- Physics scenes now have world gravity, defaulting to `(0, -10)`. If you previously added a `Gravity` force generator to every body to get gravity, remove it or set the world gravity to zero with `scene->getPhysics()->setGravity(vec2(0.0f, 0.0f))`.
- The collision solver was rewritten, so restitution and friction may feel slightly different. Bounces below 1 unit per second are no longer applied, so resting bodies don't jitter.
- `KeyListener::isKeyBeginDown` and `MouseListener::isMouseBeginDown` are now true for the whole frame a key or button was pressed in, rather than only for the first caller.
- A key or mouse button pressed and released within one frame now still reports `isKeyBeginDown`/`isMouseBeginDown` for that frame, along with the new `isKeyEndDown`/`isMouseEndDown`.
- `Pancake::load` and `Window::load` take a `std::function<void(Scene*)>` instead of a function pointer. Existing calls still compile.

## Contributions
Contributions are welcome! If you find any bugs or have suggestions for improvements, feel free to open an issue or submit a pull request on GitHub.

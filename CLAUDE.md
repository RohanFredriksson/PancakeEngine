# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Pancake is a small 2D C++ game engine built as a **static library** (`pancake`). There is no executable target in this repo: games are built from a separate template project (https://github.com/RohanFredriksson/PancakeTemplate) that adds this repo as a CMake subdirectory and links `pancake`. Dependencies: GLFW, GLM, glad (vendored), stb (vendored), nlohmann/json, Dear ImGui, SoLoud (author's fork).

## Build

```bash
git submodule update --init --recursive   # glfw, glm, imgui, json, soloud are submodules
cmake -S . -B build -DGLFW_BUILD_WAYLAND=OFF
make -C build -j8 pancake
```

- On Linux, GLFW needs X11 dev headers (`libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`) and SoLoud needs `libasound2-dev`.
- No C++ standard is set; this relies on the compiler default (C++17 on GCC 13). `spatial.hpp` uses `SpatialHashGrid<T>(float)` as a constructor name, which is invalid in C++20.
- `CMAKE_EXE_LINKER_FLAGS` adds `-fsanitize=address` on non-Windows, so anything linking the library gets ASan at link time.
- `src/*.cpp` is picked up by `GLOB_RECURSE`; re-run cmake after adding files.

## Tests / running

There is no test suite, linter, or CI. To exercise the engine, write a small executable that links `pancake` (`add_subdirectory(<repo> pancake)` + `target_link_libraries(x pancake)`), call `Pancake::load(initFn); Pancake::start();`, and run it headless with `xvfb-run -a ./x` (Mesa llvmpipe provides GL 3.3 core). Drive it from a `Component::update` that calls `Pancake::stop()` after N frames. ALSA/JACK warnings without a sound card are harmless.

## Architecture

**Engine/Window loop** (`core/engine.cpp`, `core/window.cpp`). `Pancake::*` free functions are a thin facade over the `Window` namespace, which holds all global state (GLFW window, current `Scene*`, shaders, entity-picking framebuffer). Calls such as `load`, `save`, `reset`, `title`, and `size` only **set flags**; the main loop applies them at the top of the next frame. `load(fn)` runs the init function against the current scene, while `reset()` deletes the scene and creates a new empty one. Each frame does `scene->update(dt)`, then renders twice: once into an RGB32F framebuffer with the entity shader (entity IDs for mouse picking via `Window::readPixel`) and once to the screen. After that come `DebugDraw::render()` and the ImGui console.

**Scene → Entity → Component** (`core/`). `Scene` owns `Entity*`s plus a `Camera`, a `Renderer` (sprite batches) and a physics `World`. Entities own `Component*`s. IDs come from static counters, and loading from JSON bumps the counter past loaded IDs. Lifecycle:
- `start()` runs when the entity is added to a started scene (or on `Scene::start`). Components **self-register** with scene subsystems in `start()` through `Window::getScene()` (e.g. `SpriteRenderer` → `getRenderer()->add`, `Rigidbody` → `getPhysics()->add`, `ForceGenerator` → `getPhysics()->addForceGenerator`) and unregister in `end()`.
- `kill()` marks the component dead and calls `end()`. The owning `update` loop deletes dead entities and components later. `Entity::~Entity` deletes its components.
- `TransformableComponent` adds a position/size/rotation offset relative to the entity. `Entity::addRotation` rotates the offsets of all transformable components.

Because of the reliance on the global `Window::getScene()`, components assume they belong to the *current* scene, including during destruction.

**Serialisation / reflection** (`core/factory.hpp`). `REGISTER(Base, Derived)` creates a static `Creator` that registers a default constructor under the class name. `FACTORY(Component).create(type)` is used by `Entity::load`, and `FACTORY(Collider)` by `Rigidbody::load`. Every serialisable component needs a default constructor, a `serialise()`/`load(json)` pair that chains to its base class, and a `REGISTER` line in its header. `Scene::serialise` also writes the asset pools (fonts, spritesheets, sprites, audio) so a save file is self-contained.

**Physics** (`physics/`). `Rigidbody` is a component that holds non-component `Collider`s (`BoxCollider`, `CircleCollider`, owned by the rigidbody). Mass, centroid, moment and bounds are lazily recomputed via dirty flags. `World::update` steps at a fixed timestep (1/60) and does the following:
- integrates bodies, then culls pairs with `SpatialHashGrid<Rigidbody*>` (cell size 4); bodies update their grid cells when their bounds change in `physicsUpdate`
- runs narrow phase (`Collision::findCollisionFeatures`, SAT-style box/box, circle/circle, circle/box) and notifies any component on either entity that also inherits `CollisionListener`
- applies forces through `ForceRegistry`
- resolves impulses iteratively (6 iterations)

Forces are opt-in per body: a rigidbody lists force-generator type names (`addForceGenerator("Gravity")`), and a `ForceGenerator` component somewhere in the scene with that type name provides the force. There is **no gravity by default**; the `World` constructor's gravity argument is unused. Bodies with zero total collider mass are treated as infinite-mass statics. `sensor` bodies report collisions but get no impulse.

**Rendering** (`graphics/`). `Renderer` groups `SpriteRenderer`s into `RenderBatch`es by z-index (max 1000 quads and 7 textures per batch). `TextRenderer` and `Animation` produce child `SpriteRenderer`s. Shader sources are string constants in `asset/shaders.hpp`, and the default font (Pixellari) is embedded as a byte array in `asset/fonts.hpp`. Assets are cached by name in the namespaces in `asset/assetpool.hpp` (`TexturePool`, `SpritePool`, `FontPool`, `AudioPool`). `DebugDraw` is immediate-mode lines and boxes with a frame lifetime.

**Audio** (`audio/`). A SoLoud wrapper. `AudioEngine::init/destroy` wraps `Window::start()` in `Pancake::start()`. `AudioPlayer` is the component and `AudioWave` is the pooled asset.

## Conventions

- Namespace `Pancake`. Internal helpers live in anonymous namespaces inside `.cpp` files. Headers include their dependents at the *bottom* (e.g. `component.hpp` includes `entity.hpp` after the class) to break include cycles.
- The recent commits ("The war against using has begun") have been removing `using` declarations in favour of fully qualified `std::`/`glm::` names; follow that in touched code. Some headers still export `using std::string; using glm::vec2;` etc., and other code relies on that.
- Single-line `if (...) {...}` braces; British spelling (`serialise`, `colour`, `initialiser`).
- Errors are printed as `ERROR::AREA::DETAIL` to stdout; load functions return `false`/`nullptr` on malformed JSON instead of throwing.

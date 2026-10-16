# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Pancake is a small 2D C++17 game engine built as a **static library** (`pancake`). Games are normally built from a separate template project (https://github.com/RohanFredriksson/PancakeTemplate) that adds this repo as a CMake subdirectory and links `pancake`. This repo has its own tests and a demo sandbox in `examples/demo.cpp`. Dependencies: GLFW, GLM, glad (vendored), stb (vendored), nlohmann/json, Dear ImGui, SoLoud (author's fork).

## Build and test

```bash
git submodule update --init --recursive
cmake -S . -B build -DGLFW_BUILD_WAYLAND=OFF            # add -DPANCAKE_ASAN=ON for AddressSanitizer
cmake --build build -j
ctest --test-dir build --output-on-failure              # all tests
ctest --test-dir build -L unit                          # only the tests that need no window
ctest --test-dir build -R test_physics                  # one test executable
./build/tests/test_physics                              # run directly to see each case
./build/examples/pancake_demo [--frames N]
```

- On Linux, install `libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev libasound2-dev xvfb`. With CMake 4+, add `-DCMAKE_POLICY_VERSION_MINIMUM=3.5`, because the bundled glad/glm/json declare old minimums.
- Options `PANCAKE_BUILD_TESTS` and `PANCAKE_BUILD_EXAMPLES` default to on only when this is the top level project. `PANCAKE_ASAN` applies to `pancake` publicly, so test and demo binaries get it too.
- CI (`.github/workflows/ci.yml`) runs an ASan build and a Release build, both on ubuntu-24.04 under Xvfb.

### Tests

There is no external test framework. `tests/test.hpp` provides `TEST("name") {...}`, `CHECK`, `CHECK_NEAR` and `RUN_TESTS()`. Each `tests/test_*.cpp` is its own executable, registered in `tests/CMakeLists.txt`:
- **Unit tests** (`pancake_test`) build physics bodies and a `World` directly, with no window and no scene. Use `tests/helpers.hpp` (`PancakeTest::Bodies`), which owns the entities. LeakSanitizer runs with `tests/lsan.supp`, which suppresses leaks inside audio and GL drivers.
- **Engine tests** (`pancake_engine_test`, and `demo_smoke` in `examples/`) run the real loop in a window, wrapped in `xvfb-run` when it is available. Leak detection is off for these, because Mesa leaks from libraries it unloads, which can't be suppressed by name. `tests/test_engine.cpp` scripts the loop from a `Driver` component. Physics runs in real time, so it waits for a condition (the box settling) and then times later steps relative to that, never to absolute frame numbers.

## Architecture

**Engine/Window loop** (`core/engine.cpp`, `core/window.cpp`). `Pancake::*` free functions are a thin facade over the `Window` namespace, which holds all global state (GLFW window, current `Scene*`, shaders, entity-picking framebuffer).
- Calls such as `load`, `save`, `reset`, `title`, and `size` only **set flags**, which the main loop applies at the top of the next frame. `load(fn)` runs the init function against the current scene, and `reset()` replaces the scene with an empty one.
- A component that resets the scene deletes itself, so tests and demos must `load` a new driver in the same frame.
- Gamepads are polled after `glfwPollEvents` each frame (`GamepadListener::poll`). Begin and end down compare against the previous poll, so they need no `endFrame`, and `setState` lets tests fake a pad.
- Each frame does `scene->update(dt)`, where `dt` is the frame time × time scale, zero while paused, or exactly one physics step for each `step()` while paused. Components keep updating while paused. Then the frame renders twice: once into an RGB32F framebuffer with the entity shader (sprite entity IDs for `Window::readPixel`) and once to the screen. Then `DebugDraw::render()` runs, followed by ImGui (`Console::render`, `Inspector::render`).
- Shutdown deletes the scene, then the GL resources, then ImGui.

**Scene → Entity → Component** (`core/`). `Scene` owns `Entity*`s plus a `Camera`, a `Renderer` (sprite batches), a physics `World`, and a `SpatialHashGrid<Entity*>`. That grid is refreshed every frame for entities whose bounds (transform plus rigidbody) changed, and it backs `getEntitiesIn/Near/At`. Entities own `Component*`s, looked up by type name (`getComponent("Rigidbody")`) or by class (`getComponent<Rigidbody>()`, which skips dead components and matches subclasses). Entities have an optional `name`, found with `Scene::findEntity`. IDs come from static counters, and loading from JSON bumps the counter past loaded IDs. Lifecycle:
- `start()` runs when the entity is added to a started scene. Most components self-register with scene systems in `start()` through the global `Window::getScene()` (e.g. `SpriteRenderer` → `getRenderer()->add`, `Rigidbody` → `getPhysics()->add`) and unregister in `end()`.
- `Entity::update` only updates the components it had when the frame began, so a component added during an update (e.g. a chained `Tween`) first runs next frame.
- `Scene::after`/`every` timers run after entity updates, advance with the scene `dt`, and are cancelled when their owner entity dies. They aren't serialised. `Tween` (`components/tween.hpp`) is a non-serialised component that kills itself when done, and reads its start value when it begins, after its delay.
- `kill()` marks a component dead and calls `end()`. The owning update loop deletes dead entities and components afterwards.
- `Scene::~Scene` kills every entity before deleting any systems, so components can unregister while the systems still exist.
- `Camera::shake` offsets the view only (`getView`). `getPosition` stays the logical position, which `CameraFollow` and mouse world coordinates use. `CameraFollow` (`components/camerafollow.hpp`) is serialised; its logic is in `apply(camera, target, dt)` so it can be tested without a window.
- `TransformableComponent` adds a position/size/rotation offset relative to the entity.

**Prefabs and copies** (`core/prefab.cpp`). `Entity::load(json)` keeps the ids in the json. `Entity::instantiate(json)` and `Entity::clone()` give the entity and its components new ids (a component keeps the id its constructor gave it). `Prefab::load/spawn` cache each file's json and read it again when its modified time changes.

**Serialisation / reflection** (`core/factory.hpp`). `REGISTER(Base, Derived)` registers a default constructor under the class name. `FACTORY(Component).create(type)` is used by `Entity::load`, and `FACTORY(Collider)` by `Rigidbody::load`. Every serialisable component needs a default constructor, a `serialise()`/`load(json)` pair that chains to its base class, and a `REGISTER` line. New fields added to existing components should be optional on load, so older save files keep loading (see `gravityScale`). `Scene::serialise` also writes gravity and the asset pools (fonts, spritesheets, sprites, audio).

**Physics** (`physics/`). `Rigidbody` is a component holding non-component `Collider`s (`BoxCollider`, `CircleCollider`), which it owns. Mass, centroid, moment and bounds are lazily recomputed via dirty flags. A body with zero total collider mass is static.
- Rigidbodies and `ForceGenerator`s keep a `World*` (set by `World::add`/`addForceGenerator`) and unregister through it, not through the global scene. The `World` never deletes them, because their entities own them.
- `World::fixedUpdate` runs at 1/60 s, at most 5 steps per frame:
  1. Force generators add forces.
  2. `integrateVelocity` adds forces and world gravity × `gravityScale`.
  3. Bodies moved by game code update their grid cells. Pairs are found through the `SpatialHashGrid<Rigidbody*>`, ordered by id, and skipped unless `canCollideWith` (each layer is in the other's mask).
  4. Narrow phase runs (`collision.cpp`). Listener events (enter, contact) are queued, not sent.
  5. Velocities are solved with warm-started sequential impulses, using a contact cache keyed by body id pair.
  6. `integratePosition` moves the bodies.
  7. Positions are corrected once.
  8. Exits are found by comparing against `touching` (pairs touching after the last step), then events are sent in body-id-pair order. A pair joins `touching` only when its enter is sent, and `World::remove` sends exits for the removed body's pairs. So a listener can kill bodies mid-dispatch, and exits always match delivered enters.

  This order matters: integrating positions before solving made stacks jitter and collapse.
- Collision features use one convention: the normal points from the first collider to the second, depth is the full penetration, and the point lies midway between the surfaces.
- `Scene::~Scene` calls `World::clearContacts()` before killing entities, so teardown sends no exits. `Entity::kill` and `~Entity` mark the entity dead before ending components, and listeners on dead entities or dead components are skipped.
- Forces are opt-in per body: `rigidbody->addForceGenerator("Gravity")` subscribes the body to the generator component of that type name.
- `World::raycast(ray, maxDistance, mask)` walks grid cells along the ray, skipping bodies whose layer isn't in the mask.
- Physics debug drawing is off by default (`World::setDebugDraw`, console `debug`).

**Rendering** (`graphics/`). `Renderer` groups `SpriteRenderer`s into `RenderBatch`es by z-index (max 1000 quads and 7 textures per batch). `TextRenderer` and `Animation` create child `SpriteRenderer`s. Shader sources are constants in `asset/shaders.hpp`, and the default font (Pixellari) is embedded in `asset/fonts.hpp`. Assets are cached by name in the namespaces in `asset/assetpool.hpp`. `DebugDraw` draws immediate-mode lines with a lifetime in frames.

**Hot reload** (`asset/assetpool.cpp`). `TexturePool::get` and `AudioPool::get` record each file's modified time. `AssetPool::reloadChanged()` reloads changed files in place (`Texture::reload` keeps the GL texture id, so sprites keep working), and `AssetPool::update` calls it twice a second of real time when `setHotReload(true)`.

**Tools** (`core/console.cpp`, `core/inspector.cpp`). The console has a command registry (`Console::registerCommand`, `Console::execute`). The inspector picks the clicked entity with the picking texture, falling back to scene bounds for entities without sprites. It stores the selection as an id so it never dereferences a deleted entity, and it draws each component's `imgui()`. Dragging moves the selection and stops its body. The editing actions are public (`Inspector::duplicateSelected`, `deleteSelected`, `moveSelected`) so tests can call them.

**Audio** (`audio/`). A SoLoud wrapper. `AudioEngine::init/destroy` wraps `Window::start()` in `Pancake::start()`.

## Conventions

- Namespace `Pancake`. Internal helpers live in anonymous namespaces inside `.cpp` files. Headers include their dependents at the *bottom* (e.g. `component.hpp` includes `entity.hpp` after the class) to break include cycles.
- Include `<glad/glad.h>` before anything that pulls in GLFW (including `pancake/core/listener.hpp`), or glad's header guard errors.
- Prefer fully qualified `std::`/`glm::` names in new code. Some headers still export `using std::string; using glm::vec2;` etc., and other code relies on that.
- Single-line `if (...) {...}` braces; British spelling (`serialise`, `colour`).
- `ROADMAP.md` is the queue of planned work, in batches. Tick items off in the branch that implements them.
- Errors are printed as `ERROR::AREA::DETAIL` to stdout. Load functions return `false`/`nullptr` on malformed JSON instead of throwing.

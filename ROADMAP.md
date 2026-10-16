# Roadmap

Planned improvements for using Pancake as a base for games, grouped into batches of related items.
Tick an item off in the same branch that implements it.

## Batch 1: entity and input API

- [x] Typed component access: `getComponent<T>()`, `getComponents<T>()`, `hasComponent<T>()`, `addComponent<T>(args...)`.
- [x] Release events: `KeyListener::isKeyEndDown`, `MouseListener::isMouseEndDown`. Taps that press and release within one frame still register as begin down.
- [x] Entity names and lookup: optional serialised `name`, `Scene::findEntity(name)`, `Scene::findEntities(name)`, `Scene::getEntitiesWith<T>()`. Names show in the inspector and the `entities` command.
- [x] `load` accepts any callable (`std::function<void(Scene*)>`), so capturing lambdas work.

## Batch 2: time and debugging

- [x] Time scale and pause: `Pancake::timeScale(float)`, `Pancake::pause(bool)`, `Pancake::step()`, console `pause` / `step` / `timescale`.
- [x] More console commands: `spawn`, `kill <id|name>`, `tp <id|name> <x> <y>`, `fps` overlay, `vsync on|off`, `fullscreen`.
- [x] Timers and tweens: `scene->after` / `every` / `cancel`, `Tween` for position, size, rotation, colour and custom values, with `Ease` functions.

## Batch 3: physics for gameplay

- [x] Collision enter and exit events (`collisionEnter` / `collisionExit`), from diffing contact pairs between steps. Exits are also sent when a body is removed.
- [x] Collision layers and masks on `Rigidbody`, checked in the broad phase, serialised and shown in the inspector. Raycasts take a mask.

## Batch 4: camera and input devices

- [x] `CameraFollow` component: target entity, smoothing, dead zone, bounds clamp, offset.
- [x] Camera shake: `shake(intensity, duration)`, and a `shake` console command.
- [x] Gamepad input through `glfwGetGamepadState`, with begin, down and end semantics, dead zones, and a `gamepads` console command.

## Batch 5: editor tooling

- [x] `Entity::clone()` and prefabs: `Pancake::spawn("prefab.json", position)`, `Prefab::save`, console `clone` / `prefab` / `saveprefab`.
- [x] Inspector edit mode: drag to move, delete, duplicate (buttons, Delete, Ctrl+D), play/pause/step, save and load buttons.
- [x] Hot reload of textures and audio when their files change (`Pancake::hotReload`, console `hotreload` / `reload`). Prefabs are always read again when they change.

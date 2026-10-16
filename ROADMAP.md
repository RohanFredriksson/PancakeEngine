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

- [ ] Collision enter and exit events (`collisionEnter` / `collisionExit`), from diffing contact pairs between steps.
- [ ] Collision layers and masks on `Rigidbody`, checked in the broad phase, serialised and shown in the inspector.

## Batch 4: camera and input devices

- [ ] `CameraFollow` component: target entity, smoothing, dead zone, bounds clamp.
- [ ] Camera shake: `shake(intensity, duration)`.
- [ ] Gamepad input through `glfwGetGamepadState`, with begin, down and end semantics.

## Batch 5: editor tooling

- [ ] `Entity::clone()` and prefabs: `Pancake::spawn("prefab.json", position)`.
- [ ] Inspector edit mode: drag to move, delete, duplicate, play/pause, save and load buttons.
- [ ] Hot reload of textures and audio when their files change.

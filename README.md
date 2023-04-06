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

## Example
```

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
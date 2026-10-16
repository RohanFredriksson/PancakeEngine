#include <map>
#include <sstream>
#include <cstring>
#include <imgui.h>

#include "pancake/core/console.hpp"
#include "pancake/core/inspector.hpp"
#include "pancake/core/window.hpp"
#include "pancake/graphics/debugdraw.hpp"
#include "pancake/graphics/spriterenderer.hpp"
#include "pancake/physics/rigidbody.hpp"
#include "pancake/physics/collider.hpp"

namespace Pancake {

    namespace {

        struct Entry {
            std::string description;
            Console::Command command;
        };

        bool active = false;
        bool fpsOverlay = false;
        bool scrollToBottom = false;
        std::vector<std::string> output;
        std::map<std::string, Entry> commands;
        char input[256];

        std::vector<std::string> tokenise(std::string line) {
            std::vector<std::string> tokens;
            std::istringstream stream(line);
            std::string token;
            while (stream >> token) {tokens.push_back(token);}
            return tokens;
        }

        // Parses "on", "off", or toggles the current value when no argument is given.
        bool parseState(std::vector<std::string>& args, bool current) {
            if (args.empty()) {return !current;}
            return args[0] == "on" || args[0] == "true" || args[0] == "1";
        }

        bool parseFloat(std::string text, float* value) {
            try {
                size_t used;
                *value = std::stof(text, &used);
                return used == text.size();
            } catch (const std::exception& e) {return false;}
        }

        // Finds a live entity by id, or by name when the argument isn't a number.
        Entity* findEntity(std::string idOrName) {
            Scene* scene = Window::getScene();
            if (scene == nullptr) {return nullptr;}
            Entity* e = nullptr;
            try {
                size_t used;
                int id = std::stoi(idOrName, &used);
                if (used == idOrName.size()) {e = scene->getEntity(id);}
            } catch (const std::exception& ex) {}
            if (e == nullptr) {e = scene->findEntity(idOrName);}
            if (e != nullptr && e->isDead()) {return nullptr;}
            return e;
        }

        void renderOverlay() {
            if (!fpsOverlay) {return;}
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
                                   | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 10.0f, 10.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            ImGui::SetNextWindowBgAlpha(0.5f);
            if (ImGui::Begin("##FPS", nullptr, flags)) {
                float fps = Window::getFps();
                ImGui::Text("%.0f FPS (%.2f ms)", fps, fps > 0.0f ? 1000.0f / fps : 0.0f);
                if (Window::isPaused()) {ImGui::Text("Paused");}
                if (Window::getTimeScale() != 1.0f) {ImGui::Text("Time scale %.2f", Window::getTimeScale());}
            }
            ImGui::End();
        }

        void registerDefaults() {

            static bool registered = false;
            if (registered) {return;}
            registered = true;

            Console::registerCommand("help", "List all commands.", [](std::vector<std::string> args) {
                for (auto const& c : commands) {Console::print(c.first + " - " + c.second.description);}
            });

            Console::registerCommand("quit", "Stop the engine.", [](std::vector<std::string> args) {
                Window::stop();
            });

            Console::registerCommand("clear", "Clear the console output.", [](std::vector<std::string> args) {
                Console::clear();
            });

            Console::registerCommand("reset", "Replace the scene with an empty one.", [](std::vector<std::string> args) {
                Window::reset();
            });

            Console::registerCommand("save", "save <file>: Save the scene to a file.", [](std::vector<std::string> args) {
                if (args.size() != 1) {Console::print("Usage: save <file>"); return;}
                Window::save(args[0]);
                Console::print("Saving scene to " + args[0]);
            });

            Console::registerCommand("load", "load <file>: Load a scene file into the current scene.", [](std::vector<std::string> args) {
                if (args.size() != 1) {Console::print("Usage: load <file>"); return;}
                Window::load(args[0]);
            });

            Console::registerCommand("debug", "debug [on|off]: Draw physics colliders and contacts.", [](std::vector<std::string> args) {
                Scene* scene = Window::getScene();
                if (scene == nullptr) {return;}
                bool state = parseState(args, scene->getPhysics()->hasDebugDraw());
                scene->getPhysics()->setDebugDraw(state);
                Console::print(std::string("Physics debug drawing ") + (state ? "on" : "off"));
            });

            Console::registerCommand("gravity", "gravity [x y]: Show or set the world gravity.", [](std::vector<std::string> args) {
                Scene* scene = Window::getScene();
                if (scene == nullptr) {return;}
                if (args.size() == 2) {
                    try {scene->getPhysics()->setGravity(glm::vec2(std::stof(args[0]), std::stof(args[1])));}
                    catch (const std::exception& e) {Console::print("Usage: gravity <x> <y>"); return;}
                }
                glm::vec2 g = scene->getPhysics()->getGravity();
                Console::print("Gravity: " + std::to_string(g.x) + ", " + std::to_string(g.y));
            });

            Console::registerCommand("entities", "List the entities in the scene.", [](std::vector<std::string> args) {
                Scene* scene = Window::getScene();
                if (scene == nullptr) {return;}
                for (Entity* e : scene->getEntities()) {
                    std::string line = std::to_string(e->getId());
                    if (!e->getName().empty()) {line += " " + e->getName();}
                    line += ":";
                    for (Component* c : e->getComponents()) {line += " " + c->getType();}
                    Console::print(line);
                }
            });

            Console::registerCommand("inspector", "inspector [on|off]: Show the entity inspector.", [](std::vector<std::string> args) {
                Inspector::set(parseState(args, Inspector::opened()));
            });

            Console::registerCommand("pause", "pause [on|off]: Pause or resume the scene.", [](std::vector<std::string> args) {
                bool state = parseState(args, Window::isPaused());
                Window::setPaused(state);
                Console::print(state ? "Paused" : "Resumed");
            });

            Console::registerCommand("step", "step [frames]: Pause, then advance one physics step per frame.", [](std::vector<std::string> args) {
                int frames = 1;
                if (!args.empty()) {
                    try {frames = std::stoi(args[0]);}
                    catch (const std::exception& e) {Console::print("Usage: step [frames]"); return;}
                }
                Window::step(frames);
            });

            Console::registerCommand("timescale", "timescale [scale]: Show or set how fast the scene runs.", [](std::vector<std::string> args) {
                float scale;
                if (args.size() == 1) {
                    if (!parseFloat(args[0], &scale) || scale < 0.0f) {Console::print("Usage: timescale <scale>"); return;}
                    Window::setTimeScale(scale);
                }
                Console::print("Time scale: " + std::to_string(Window::getTimeScale()));
            });

            Console::registerCommand("spawn", "spawn <x> <y> [w h]: Spawn a dynamic box.", [](std::vector<std::string> args) {
                Scene* scene = Window::getScene();
                if (scene == nullptr) {return;}
                float x, y;
                float w = 1.0f, h = 1.0f;
                bool valid = (args.size() == 2 || args.size() == 4) && parseFloat(args[0], &x) && parseFloat(args[1], &y);
                if (valid && args.size() == 4) {valid = parseFloat(args[2], &w) && parseFloat(args[3], &h) && w > 0.0f && h > 0.0f;}
                if (!valid) {Console::print("Usage: spawn <x> <y> [w h]"); return;}

                Entity* e = new Entity(glm::vec2(x, y), glm::vec2(w, h), 0.0f);
                e->addComponent<SpriteRenderer>();
                BoxCollider* box = new BoxCollider();
                box->setSize(w, h);
                box->setMass(w * h);
                e->addComponent<Rigidbody>()->addCollider(box);
                scene->addEntity(e);
                Console::print("Spawned entity " + std::to_string(e->getId()));
            });

            Console::registerCommand("kill", "kill <id|name>: Remove an entity.", [](std::vector<std::string> args) {
                if (args.size() != 1) {Console::print("Usage: kill <id|name>"); return;}
                Entity* e = findEntity(args[0]);
                if (e == nullptr) {Console::print("No entity " + args[0]); return;}
                e->kill();
                Console::print("Killed entity " + std::to_string(e->getId()));
            });

            Console::registerCommand("tp", "tp <id|name> <x> <y>: Move an entity and stop it.", [](std::vector<std::string> args) {
                float x, y;
                if (args.size() != 3 || !parseFloat(args[1], &x) || !parseFloat(args[2], &y)) {Console::print("Usage: tp <id|name> <x> <y>"); return;}
                Entity* e = findEntity(args[0]);
                if (e == nullptr) {Console::print("No entity " + args[0]); return;}
                e->setPosition(glm::vec2(x, y));
                Rigidbody* r = e->getComponent<Rigidbody>();
                if (r != nullptr) {r->setVelocity(0.0f, 0.0f)->setAngularVelocity(0.0f);}
            });

            Console::registerCommand("fps", "fps [on|off]: Show the frame rate.", [](std::vector<std::string> args) {
                fpsOverlay = parseState(args, fpsOverlay);
            });

            Console::registerCommand("vsync", "vsync [on|off]: Limit the frame rate to the display.", [](std::vector<std::string> args) {
                bool state = parseState(args, Window::hasVsync());
                Window::setVsync(state);
                Console::print(std::string("Vsync ") + (state ? "on" : "off"));
            });

            Console::registerCommand("fullscreen", "fullscreen [on|off]: Fill the primary monitor.", [](std::vector<std::string> args) {
                Window::setFullscreen(parseState(args, Window::isFullscreen()));
            });

        }

    }

    namespace Console {

        void open() {
            active = true;
        }

        void close() {
            active = false;
        }

        void set(bool state) {
            active = state;
        }

        void toggle() {
            active = !active;
        }

        void render() {

            renderOverlay();
            
            // If not active, don't render the terminal.
            if (!active) {return;}
            // Open in the bottom left corner, clear of the inspector.
            ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 310.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(480, 300), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Console", &active)) {ImGui::End(); return;}

            // Create a scrollable text area for the terminal output
            float footer = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            ImGui::BeginChild("Scrolling", ImVec2(0, -footer));
            for (const auto& message : output) {ImGui::TextUnformatted(message.c_str());}
            if (scrollToBottom) {ImGui::SetScrollHereY(1.0f); scrollToBottom = false;}
            ImGui::EndChild();

            // Create a text input box for entering commands, executed when enter is pressed.
            if (ImGui::InputText("Command", input, sizeof(input), ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(input) > 0) {
                    output.push_back("> " + std::string(input));
                    execute(input);
                    memset(input, 0, sizeof(input));
                    scrollToBottom = true;
                }
                ImGui::SetKeyboardFocusHere(-1);
            }

            ImGui::End();

        }

        bool opened() {
            return active;
        }

        void execute(std::string line) {

            registerDefaults();
            std::vector<std::string> tokens = tokenise(line);
            if (tokens.empty()) {return;}

            auto it = commands.find(tokens[0]);
            if (it == commands.end()) {
                print("Unknown command. Type 'help' for a list of available commands.");
                return;
            }

            std::vector<std::string> args(tokens.begin() + 1, tokens.end());
            it->second.command(args);

        }

        void registerCommand(std::string name, std::string description, Command command) {
            registerDefaults();
            commands[name] = {description, command};
        }

        void print(std::string message) {
            output.push_back(message);
            scrollToBottom = true;
        }

        void clear() {
            output.clear();
        }

    }

}

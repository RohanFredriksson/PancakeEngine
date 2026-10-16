#include <map>
#include <sstream>
#include <cstring>
#include <imgui.h>

#include "pancake/core/console.hpp"
#include "pancake/core/inspector.hpp"
#include "pancake/core/window.hpp"
#include "pancake/graphics/debugdraw.hpp"

namespace Pancake {

    namespace {

        struct Entry {
            std::string description;
            Console::Command command;
        };

        bool active = false;
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
                    std::string line = std::to_string(e->getId()) + ":";
                    for (Component* c : e->getComponents()) {line += " " + c->getType();}
                    Console::print(line);
                }
            });

            Console::registerCommand("inspector", "inspector [on|off]: Show the entity inspector.", [](std::vector<std::string> args) {
                Inspector::set(parseState(args, Inspector::opened()));
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

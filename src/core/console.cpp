#include "pancake/core/console.hpp"
#include "pancake/core/window.hpp"
#include <imgui.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace Pancake {

    namespace {

        bool active = true;
        vector<string> output;
        char input[256];

        void parse(const char* command) {

            if (strcmp(command, "help") == 0) {output.push_back("Available commands: help, quit");}
            else if (strcmp(command, "quit") == 0) {Window::stop();}
            else {output.push_back("Unknown command. Type 'help' for a list of available commands.");}

        }

    }

    namespace Console {

        void open() {
            active = true;
        }

        void close() {
            active = false;
        }

        void render() {
            
            // If not active, don't render the terminal.
            if (!active) {return;}
            ImGui::Begin("Console");

            // Create a scrollable text area for the terminal output
            ImGui::BeginChild("Scrolling");
            for (const auto& message : output) {ImGui::Text(message.c_str());}
            ImGui::EndChild();

            // Create a text input box for entering commands
            ImGui::InputText("Command", input, sizeof(input));

            // If the enter key is pressed, parse the command and add it to the terminal buffer
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) && strlen(input) > 0) {
                output.push_back("> " + std::string(input));
                parse(input);
                memset(input, 0, sizeof(input));
            }

            ImGui::End();

        }

    }

}


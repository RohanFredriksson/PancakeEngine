#pragma once

#include <string>
#include <vector>
#include <functional>

namespace Pancake {

    namespace Console {

        // A command receives its arguments, excluding the command name itself.
        typedef std::function<void(std::vector<std::string> args)> Command;

        void open();
        void close();
        void set(bool state);
        void toggle();
        void render();
        bool opened();

        // Run a command as if it was typed into the console.
        void execute(std::string line);

        // Add a command, replacing any existing command with the same name.
        void registerCommand(std::string name, std::string description, Command command);

        // Write a line to the console output.
        void print(std::string message);
        void clear();

    }

}

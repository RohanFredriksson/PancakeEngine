#pragma once

#include <glm/glm.hpp>

namespace Pancake {

    // An ImGui window for viewing and editing the scene. While open, left clicking in the game selects the
    // entity under the mouse and shows the editor for each of its components, and dragging moves it.
    // Delete removes the selected entity and Ctrl+D duplicates it. The toolbar pauses, steps, saves and loads.
    namespace Inspector {
        void open();
        void close();
        void set(bool state);
        void toggle();
        bool opened();
        void render();

        int getSelected();
        void select(int entityId);

        // Editing actions on the selected entity, as the buttons and shortcuts do.
        int duplicateSelected();              // Selects and returns the copy's id, or -1.
        bool deleteSelected();
        void moveSelected(glm::vec2 delta);   // Bodies are stopped, so they don't fly off when let go.
    }

}

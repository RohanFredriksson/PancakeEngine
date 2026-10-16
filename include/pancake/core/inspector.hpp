#pragma once

namespace Pancake {

    // An ImGui window for viewing and editing the scene. While open, left clicking in the
    // game selects the entity under the mouse and shows the editor for each of its components.
    namespace Inspector {
        void open();
        void close();
        void set(bool state);
        void toggle();
        bool opened();
        void render();

        int getSelected();
        void select(int entityId);
    }

}

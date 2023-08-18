#pragma once

namespace Pancake {

    namespace Console {
    
        void open();
        void close();
        void set(bool state);
        void toggle();
        void render();
        bool opened();

    }

}
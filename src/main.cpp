#include "audio/audioengine.hpp"
#include "window/window.hpp"

int main() {

    AudioEngine::init();

    Window::init();
    Window::loop();
    Window::destroy();

    AudioEngine::destroy();
}
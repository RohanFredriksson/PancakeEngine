/*#include "window/window.hpp"

int main() {
    Window::init();
    Window::loop();
    Window::destroy();
}
*/




#include "soloud/include/soloud_thread.h"
#include "audio/audioengine.hpp"
#include "audio/audiowave.hpp"

int main() {

    AudioEngine::init();
    AudioWave audio("assets/audio/ui.mp3");
    AudioEngine::play(&audio);

    // Wait until sounds have finished
    while (AudioEngine::getActiveVoiceCount() > 0) {
        SoLoud::Thread::sleep(100);
    }

    AudioEngine::destroy();
    return 0;

}
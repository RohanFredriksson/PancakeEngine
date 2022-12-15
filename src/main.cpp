/*#include "window/window.hpp"

int main() {
    Window::init();
    Window::loop();
    Window::destroy();
}
*/

#include <stb/stb_vorbis.h>

#include "soloud/include/soloud.h"
#include "soloud/include/soloud_wav.h"
#include "soloud/include/soloud_speech.h"
#include "soloud/include/soloud_thread.h"

int main() {

    // Define a couple of variables
    SoLoud::Soloud soloud;  // SoLoud engine core
    SoLoud::Wav source; // One wave file

    source.load("assets/audio/ui.mp3");

    // initialize SoLoud.
    soloud.init();

    // Play the sound source (we could do this several times if we wanted)
    soloud.play(source);

    // Wait until sounds have finished
    while (soloud.getActiveVoiceCount() > 0)
    {
        // Still going, sleep for a bit
        SoLoud::Thread::sleep(100);
    }

    // Clean up SoLoud
    soloud.deinit();

    // All done.
    return 0;

}
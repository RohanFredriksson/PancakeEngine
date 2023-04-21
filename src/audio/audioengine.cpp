#include <soloud/include/soloud.h>
#include "pancake/audio/audioengine.hpp"

namespace Pancake {

    namespace {
        SoLoud::Soloud engine;
    }

    namespace AudioEngine {

        void init() {
            engine.init();
        }

        void destroy() {
            engine.deinit();
        }

        void play(AudioWave* audio) {
            engine.play(audio->getSource());
        }

        void stop(AudioWave* audio) {
            engine.stopAudioSource(audio->getSource());
        }

        int getActiveVoiceCount() {
            return engine.getActiveVoiceCount();
        }

    }

}
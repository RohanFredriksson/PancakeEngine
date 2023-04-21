#pragma once

#include "pancake/audio/audiowave.hpp"

namespace Pancake {

    namespace AudioEngine {
    
        void init();
        void destroy();

        void play(AudioWave* audio);
        void stop(AudioWave* audio);

        int getActiveVoiceCount();

    }

}
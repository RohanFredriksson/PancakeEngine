#pragma once

#include "audiowave.hpp"

namespace AudioEngine {
    
    void init();
    void destroy();

    void play(AudioWave* audio);
    void stop(AudioWave* audio);

    int getActiveVoiceCount();

}
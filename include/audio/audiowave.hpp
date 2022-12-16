#pragma once

#include <string>
#include "soloud/include/soloud.h"
#include "soloud/include/soloud_wav.h"

using std::string;
using SoLoud::Wav;

class AudioWave {

    private:

        Wav source;
        string filename;
        float volume;
        bool looping;

    public:

        AudioWave(string filename);
        
        Wav& getSource();
        float getVolume();
        bool isLooping();

        void setVolume(float volume);
        void setLooping(bool looping);

};
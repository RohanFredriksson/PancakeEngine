#pragma once

#include <string>
#include <soloud/include/soloud_wav.h>
#include <nlohmann/json.hpp>

using std::string;
using SoLoud::Wav;
using json = nlohmann::json;

class AudioWave {

    private:
        Wav source;
        string filename;

    public:

        AudioWave(string filename);
        json serialise();
        static void load(json j);
        
        Wav& getSource();
        string getFilename();
        float getLength();

        void setVolume(float volume);
        void setLooping(bool looping);

        void stop();

};
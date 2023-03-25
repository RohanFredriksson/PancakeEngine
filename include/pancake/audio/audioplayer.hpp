#pragma once

#include "pancake/core/component.hpp"
#include "pancake/audio/audiowave.hpp"

class AudioPlayer : public Component {

    private:
        AudioWave* audio;

    public:

        static Component* create();
        AudioPlayer();
 
        AudioWave* getAudioWave();
        string getFilename();
        float getVolume();
        bool isLooping();
        float getLength();

        AudioPlayer* setAudioWave(AudioWave* audio);
        AudioPlayer* setVolume(float volume);
        AudioPlayer* setLooping(bool looping);

        void play();
        void stop();

};
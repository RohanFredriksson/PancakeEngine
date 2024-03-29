#pragma once

#include "pancake/core/component.hpp"
#include "pancake/audio/audiowave.hpp"

namespace Pancake {

    class AudioPlayer : public Component {

        private:
            AudioWave* audio;

        public:

            AudioPlayer();
            json serialise() override;
            bool load(json j) override;
    
            AudioWave* getAudioWave();
            string getFilename();
            float getLength();

            AudioPlayer* setAudioWave(AudioWave* audio);
            AudioPlayer* setVolume(float volume);
            AudioPlayer* setLooping(bool looping);

            void play();
            void stop();

    };

    REGISTER(Component, AudioPlayer);

}
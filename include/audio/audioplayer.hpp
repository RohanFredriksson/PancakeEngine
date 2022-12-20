#pragma once

#include "audio/audiowave.hpp"

class AudioPlayer : public Component {

    private:
        AudioWave* audio;

    public:

        AudioPlayer();
        ~AudioPlayer() override;

        void update(float dt) override;
        
        // Getters
        AudioWave* getAudioWave();
        
        // Setters
        AudioPlayer* setAudioWave(AudioWave* audio);

};
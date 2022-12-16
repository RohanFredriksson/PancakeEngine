#include <algorithm>
#include <stb/stb_vorbis.h>
#include "audio/audiowave.hpp"

using std::min;
using std::max;

AudioWave::AudioWave(string filename) {
    this->source.load(filename.c_str());
}
        
Wav& AudioWave::getSource() {
    return this->source;
}

float AudioWave::getVolume() {
    return this->volume;
}

bool AudioWave::isLooping() {
    return this->looping;
}

void AudioWave::setVolume(float volume) {
    this->volume = max(min(volume, 1.0f), 0.0f);
    this->source.setVolume(double (this->volume));
}

void AudioWave::setLooping(bool looping) {
    this->looping = looping;
    this->source.setLooping(this->looping);
}
#include <iostream>
#include <algorithm>
#include "pancake/audio/audiowave.hpp"
#include "pancake/asset/assetpool.hpp"
#include "pancake/asset/audio.hpp"

using std::min;
using std::max;

AudioWave::AudioWave(string filename) {
    
    this->filename = filename;
    int result = this->source.load(filename.c_str());

    if (result != SoLoud::SO_NO_ERROR) {
        std::cout << "ERROR::AUDIOWAVE::INIT::AUDIOWAVE_LOADING_FAILED: '" << this->filename << "'\n";
        this->source.loadMem(AUDIO_EMPTY, AUDIO_EMPTY_LEN);
    }

}

json AudioWave::serialise() {
    json j;
    j.emplace("filename", this->filename);
    return j;
}

void AudioWave::load(json j) {
    if (!j.contains("filename") || !j["filename"].is_string()) {return;}    
    AudioWave* a = AudioPool::get(j["filename"]);
}
        
Wav& AudioWave::getSource() {
    return this->source;
}

string AudioWave::getFilename() {
    return this->filename;
}

float AudioWave::getLength() {
    return (float) this->source.getLength();
}

void AudioWave::setVolume(float volume) {
    volume = max(min(volume, 1.0f), 0.0f);
    this->source.setVolume(double (volume));
}

void AudioWave::setLooping(bool looping) {
    this->source.setLooping(looping);
}

void AudioWave::stop() {
    this->source.stop();
}
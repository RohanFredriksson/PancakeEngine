#include "pancake/audio/audioplayer.hpp"
#include "pancake/audio/audioengine.hpp"
#include "pancake/asset/assetpool.hpp"

namespace Pancake {

    REGISTER(Component, AudioPlayer);

    AudioPlayer::AudioPlayer() : Component("AudioPlayer") {
        this->audio = nullptr;
    }

    json AudioPlayer::serialise() {
        json j = this->Component::serialise();
        j.emplace("filename", this->getFilename());
        return j;
    }

    bool AudioPlayer::load(json j) {
        if (!this->Component::load(j)) {return false;}
        if (!j.contains("filename") || !j["filename"].is_string()) {return false;}
        AudioWave* a = AudioPool::get(j["filename"]);
        this->setAudioWave(a);
        return true;
    }

    AudioWave* AudioPlayer::getAudioWave() {
        return this->audio;
    }

    string AudioPlayer::getFilename() {
        if (this->audio == nullptr) {return "nullptr";}
        return this->audio->getFilename();
    }

    float AudioPlayer::getLength() {
        if (this->audio == nullptr) {return -1.0f;}
        return this->audio->getLength();
    }

    AudioPlayer* AudioPlayer::setAudioWave(AudioWave* audio) {
        this->audio = audio;
        return this;
    }

    AudioPlayer* AudioPlayer::setVolume(float volume) {
        if (this->audio != nullptr) {this->audio->setVolume(volume);}
        return this;
    }

    AudioPlayer* AudioPlayer::setLooping(bool looping) {
        if (this->audio != nullptr) {this->audio->setLooping(looping);}
        return this;
    }

    void AudioPlayer::play() {
        if (this->audio != nullptr) {AudioEngine::play(this->audio);}
    }

    void AudioPlayer::stop() {
        if (this->audio != nullptr) {this->audio->stop();}
    }

}
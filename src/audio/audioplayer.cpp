#include "pancake/audio/audioplayer.hpp"
#include "pancake/audio/audioengine.hpp"
#include "pancake/util/assetpool.hpp"

Component* AudioPlayer::create() {
    return new AudioPlayer();
}

AudioPlayer::AudioPlayer() : Component("AudioPlayer") {
    this->audio = NULL;
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
    if (this->audio == NULL) {return "NULL";}
    return this->audio->getFilename();
}

float AudioPlayer::getLength() {
    if (this->audio == NULL) {return -1.0f;}
    return this->audio->getLength();
}

AudioPlayer* AudioPlayer::setAudioWave(AudioWave* audio) {
    this->audio = audio;
    return this;
}

AudioPlayer* AudioPlayer::setVolume(float volume) {
    if (this->audio != NULL) {this->audio->setVolume(volume);}
    return this;
}

AudioPlayer* AudioPlayer::setLooping(bool looping) {
    if (this->audio != NULL) {this->audio->setLooping(looping);}
    return this;
}

void AudioPlayer::play() {
    if (this->audio != NULL) {AudioEngine::play(this->audio);}
}

void AudioPlayer::stop() {
    if (this->audio != NULL) {this->audio->stop();}
}
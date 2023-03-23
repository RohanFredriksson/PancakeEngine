#include "pancake/audio/audioplayer.hpp"
#include "pancake/audio/audioengine.hpp"

AudioPlayer::AudioPlayer() {
    this->audio = NULL;
}

AudioWave* AudioPlayer::getAudioWave() {
    return this->audio;
}

string AudioPlayer::getFilename() {
    if (this->audio == NULL) {return "NULL";}
    return this->audio->getFilename();
}

float AudioPlayer::getVolume() {
    if (this->audio == NULL) {return -1.0f;}
    return this->audio->getVolume();
}

bool AudioPlayer::isLooping() {
    if (this->audio == NULL) {return false;}
    return this->audio->isLooping();
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
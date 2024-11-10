#include "SoundManager.h"

SoundManager::SoundManager()
    : currentState(SoundState::IDLE),
      currentSequence(nullptr),
      currentToneIndex(0),
      toneStartTime(0) {}

void SoundManager::begin() {
    M5.Speaker.begin();
    M5.Speaker.setVolume(255);
}

void SoundManager::playSoundSequence(const SoundSequence* sequence) {
    if (currentState == SoundState::IDLE && sequence != nullptr) {
        currentSequence = sequence;
        currentToneIndex = 0;
        toneStartTime = millis();
        if (currentSequence->melody[0].frequency != 0) {
            M5.Speaker.tone(currentSequence->melody[0].frequency, currentSequence->melody[0].duration);
        } else {
            M5.delay(currentSequence->melody[0].duration);
        }
        currentState = SoundState::PLAYING;
    }
}

void SoundManager::update() {
    if (currentState == SoundState::PLAYING && currentSequence != nullptr) {
        unsigned long currentTime = millis();
        unsigned long elapsed = currentTime - toneStartTime;
        unsigned long currentToneDuration = currentSequence->melody[currentToneIndex].duration;

        if (elapsed >= currentToneDuration) {
            currentToneIndex++;
            if (currentToneIndex < currentSequence->length) {
                toneStartTime = currentTime;
                if (currentSequence->melody[currentToneIndex].frequency != 0) {
                    M5.Speaker.tone(currentSequence->melody[currentToneIndex].frequency,
                                   currentSequence->melody[currentToneIndex].duration);
                }
            } else {
                currentState = SoundState::IDLE;
                currentSequence = nullptr;
            }
        }
    }
}

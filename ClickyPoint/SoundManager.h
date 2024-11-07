#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <M5Unified.h>

struct ToneDuration {
    uint16_t frequency;       // in Hz, 0 for silence
    unsigned long duration;   // in milliseconds
};

struct SoundSequence {
    const ToneDuration* melody;
    size_t length;
};

class SoundManager {
public:
    SoundManager();
    void begin();
    void playSoundSequence(const SoundSequence* sequence);
    void update();

private:
    enum class SoundState {
        IDLE,
        PLAYING
    };

    SoundState currentState;
    const SoundSequence* currentSequence;
    size_t currentToneIndex;
    unsigned long toneStartTime;
};

#endif

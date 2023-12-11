#ifndef SOUNDS_H
#define SOUNDS_H
#include "../SoundManager.h"

enum class SoundType {
  STARTUP_SOUND,
  NONE
};

#define SOUND_NOTES_SIZE(options) sizeof(options) / sizeof(Note)

const Note startupSoundNotes[] = {
  {698, 400}, {880, 400}, {1080, 400}
};

const Sound sounds[] = {
  { startupSoundNotes, SOUND_NOTES_SIZE(startupSoundNotes) }
};

inline const Sound* getSound(SoundType type) {
  if (type == SoundType::NONE) {
    return nullptr;
  }

  return &sounds[(byte) type];
}

#endif // SOUNDS_H

#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H
#include <Arduino.h>

enum class SoundType;

struct Note {
  int value;
  unsigned long duration;
};

struct Sound {
  const Note* notes;
  int lenNotes;
};

class SoundManager {
  // bool enabled;
  byte activeSound = 0;
  byte currentTone = 0;
  unsigned long lastTone = 0;

public:
  SoundManager();

  void setEnabled(bool state);

  bool getEnabled() const;

  void playSound(SoundType sound);

  void runtime();

private:
  bool isPlaying() const;

  const Sound* getActiveSound() const;

  void playTone();
};

#endif // SOUND_MANAGER_H

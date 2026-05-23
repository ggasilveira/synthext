#pragma once
#include "fluidsynth.h"
#include <cstdint>
#include <fluidsynth/settings.h>
#include <string>
#include <vector>

namespace synthlib {

class Player {
public:
  Player();
  ~Player();
  /// Load a soundfont file
  void load_soundfont(const std::string &file);
  /// Load a midi file from memory to play
  void load_midi(const std::vector<uint8_t> &midifile);
  /// Start/resume playing the midi file
  void play();
  /// Stop playing the midi file
  void stop();
  /// Seek in the current playing file in ticks
  void seek(int ticks);
  /// Get the file length in ticks
  int total_ticks();
  /// Get the current tick in the file
  int current_tick();

private:
  std::vector<uint8_t> midi_file;
  fluid_settings_t *settings = nullptr;
  fluid_synth_t *synth = nullptr;
  fluid_player_t *player = nullptr;
  fluid_audio_driver_t *audio_driver = nullptr;
};

} // namespace synthlib

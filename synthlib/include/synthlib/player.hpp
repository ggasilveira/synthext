#pragma once
#include "fluidsynth.h"
#include <cstdint>
#include <fluidsynth/settings.h>
#include <string>
#include <vector>

namespace synthlib {

/// This class plays a MIDI file.
class Player {
public:
  Player();
  ~Player();
  /// Load a soundfont file.
  /// @param file the file to load.
  void load_soundfont(const std::string &file);
  /// Load a MIDI file from memory to play.
  /// @param midifile the MIDI file
  void load_midi(const std::vector<uint8_t> &midifile);
  /// Start/resume playing the midi file
  void play();
  /// Stop playing the midi file
  void stop();
  /// Seek in the current playing file in ticks.
  /// @param ticks the file position in ticks
  void seek(int ticks);
  /// Get the file length in ticks.
  /// @return the total file ticks
  int total_ticks();
  /// Get the current tick in the file.
  /// @return the current tick
  int current_tick();

  /// Returns true if the player is playing a song.
  /// @return if the player is currently playing a song
  bool is_playing();

  /// Wait for player to finish playing.
  void join();

private:
  std::vector<uint8_t> midi_file;
  fluid_settings_t *settings = nullptr;
  fluid_synth_t *synth = nullptr;
  fluid_player_t *player = nullptr;
  fluid_audio_driver_t *audio_driver = nullptr;
};

} // namespace synthlib

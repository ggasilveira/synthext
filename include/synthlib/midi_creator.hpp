#pragma once

#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace synthlib {

struct Track {
  std::vector<uint8_t> buf;
  uint32_t delta = 0;
};

/// This class is responsible for writing the MIDI file.
class MidiCreator {
public:
  /// Creates a MidiCreator
  MidiCreator(int ticks_per_beat = 1) : beat_ticks(ticks_per_beat) {}
  /// Set the writer to the specified track
  void goto_track(int track_num);
  /// Plays the note for one beat in the current track
  void play_note(Channel channel, Note note, Octave octave, Volume volume);
  /// Plays n beats of silence
  void pause_beats(uint32_t n = 1);
  /// Pauses for n ticks
  void pause_ticks(uint32_t n);
  /// Changes the track's instrument
  void change_instrument(Channel channel, Instrument instr);
  /// Sets the global bpm
  void set_bpm(Bpm bpm);

  std::vector<uint8_t> generate_file();

private:
  std::vector<Track> tracks;
  int curr_track = -1;
  int beat_ticks = 1;

  void write_header(std::vector<uint8_t> &buf);
  void write_track(std::vector<uint8_t> &buf, int track);
};

} // namespace synthlib

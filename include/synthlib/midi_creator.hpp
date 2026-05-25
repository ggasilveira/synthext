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
  /// Set the writer to the specified track
  void goto_track(int track_num);
  /// Plays the note for one beat in the current track
  void play_note(Channel channel, Note note, Octave octave, Volume volume);
  /// Plays n beats of silence
  void play_pause(uint32_t n);
  /// Plays a beat of silence
  void play_pause();
  /// Changes the track's instrument
  void change_instrument(Channel channel, Instrument instr);
  /// Sets the global bpm
  void set_bpm(Bpm bpm);

  std::vector<uint8_t> generate_file();

private:
  static constexpr int BEAT_DELTA = 2;
  static constexpr uint8_t TICKS_PER_QUARTER = 2;

  std::vector<Track> tracks;
  int curr_track = -1;

  void write_header(std::vector<uint8_t> &buf);
  void write_track(std::vector<uint8_t> &buf, int track);
};

} // namespace synthlib

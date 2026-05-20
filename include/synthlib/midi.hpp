#pragma once

#include "synthlib/instrument.hpp"
#include "synthlib/octave.hpp"
#include "synthlib/volume.hpp"
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace synthlib {

enum class Note : uint8_t {
  C = 0,
  Cs = 1,
  Db = 1,
  D = 2,
  Ds = 3,
  Eb = 3,
  E = 4,
  F = 5,
  Fs = 6,
  Gb = 6,
  G = 7,
  Gs = 8,
  Ab = 8,
  A = 9,
  As = 10,
  Bb = 10,
  B = 11,
};

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
  void play_note(Note note, Octave octave, Volume volume);
  /// Plays n beats of silence
  void play_pause(uint32_t n);
  /// Plays a beat of silence
  void play_pause();
  /// Changes the track's instrument
  void change_instrument(Instrument instr);
  /// Sets the global bpm
  void set_bpm(uint32_t bpm);

  std::vector<uint8_t> generate_file();

private:
  static constexpr int BEAT_DELTA = 1;
  static constexpr uint8_t TICKS_PER_QUARTER = 1;

  std::vector<Track> tracks;
  int curr_track = -1;

  void write_header(std::vector<uint8_t> &buf);
  void write_track(std::vector<uint8_t> &buf, int track);
};

} // namespace synthlib

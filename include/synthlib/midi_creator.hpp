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
  /// Creates a MidiCreator with the amount of MIDI delta time ticks
  /// corresponding to one beat.
  /// @param ticks_per_beat number of delta time ticks per beat
  MidiCreator(int ticks_per_beat = 1) : beat_ticks(ticks_per_beat) {}
  /// Set the writer to the specified track.
  /// @param track_num track to enter
  void goto_track(int track_num);
  /// Plays the note for one beat in the current track.
  /// @param channel which channel to play the note
  /// @param note the note to play
  /// @param octave the note's octave
  /// @param the note's volume
  void play_note(Channel channel, Note note, Octave octave, Volume volume);
  /// Plays n beats of silence.
  /// @param n number of beats
  void pause_beats(uint32_t n = 1);
  /// Pauses for n ticks.
  /// @param n number of ticks
  void pause_ticks(uint32_t n);
  /// Changes the track's instrument.
  /// @param channel the channel to change the instrument
  /// @param instr the target instrument
  void change_instrument(Channel channel, Instrument instr);
  /// Issues a Tempo Change event.
  /// @param bpm the target bpm
  void set_bpm(Bpm bpm);

  /// Generates the MIDI file.
  /// @return a vector of bytes containing the generated MIDI file
  std::vector<uint8_t> generate_file();

private:
  std::vector<Track> tracks;
  int curr_track = -1;
  int beat_ticks;

  void write_header(std::vector<uint8_t> &buf);
  void write_track(std::vector<uint8_t> &buf, int track);
};

} // namespace synthlib

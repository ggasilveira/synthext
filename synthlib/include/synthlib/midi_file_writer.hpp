#pragma once

#include "synthlib/event_consumer.hpp"
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
class MidifileConsumer : IEventConsumer {
public:
  static constexpr int NO_TRACK = -1;
  /// Creates a MidiCreator with the amount of MIDI delta time ticks
  /// corresponding to one beat.
  /// @param ticks_per_beat number of delta time ticks per beat
  MidifileConsumer(int ticks_per_beat = 1) : beat_ticks(ticks_per_beat) {}
  void change_track(unsigned int track_num) override;
  void play_note(Channel channel, Note note, Octave octave,
                 Volume volume) override;
  void wait_beats(unsigned int n) override;
  void change_instrument(Channel channel, Instrument instr) override;
  void set_bpm(Bpm bpm) override;

  /// Generates the MIDI file.
  /// @return a vector of bytes containing the generated MIDI file
  std::vector<uint8_t> generate_file();

private:
  std::vector<Track> tracks;
  int curr_track = NO_TRACK;
  int beat_ticks;

  void write_header(std::vector<uint8_t> &buf);
  void write_track(std::vector<uint8_t> &buf, int track);
};

} // namespace synthlib

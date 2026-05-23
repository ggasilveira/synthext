#pragma once
#include "synthlib/midi_creator.hpp"
#include "synthlib/primitives.hpp"
#include <vector>

namespace synthlib {

/// bpm change event
struct BpmChange {
  /// absolute time in beats
  uint32_t beats;
  /// increase/decrease in bpm
  int bpm_delta;
};

/// This class manages bpm synchronization across voices.
/// The MIDI spec doesn't have a "increase bpm" command,
/// only "set bpm", so voices have to know the current bpm
/// to increase it and set to the new bpm. This requires
/// bpm updates to be ordered according to elapsed time.
class BpmManager {

public:
  /// Creates a BpmManager with the initial bpm
  BpmManager(Bpm bpm) : initial_bpm(bpm) {}
  /// Issues a bpm change in absolute time.
  void change_bpm(uint32_t beats, int delta);
  /// This function creates a dedicated track to
  /// issue bpm change events.
  void write_bpm_track(MidiCreator &creator, int track_n);

private:
  Bpm initial_bpm;
  std::vector<BpmChange> changes;
};

} // namespace synthlib

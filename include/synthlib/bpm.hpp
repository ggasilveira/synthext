#pragma once
#include "synthlib/midi.hpp"
#include <vector>

namespace synthlib {

/// bpm change event
struct BpmChange {
  /// absolute time in beats
  uint32_t beats;
  /// increase/decrease in bpm
  int32_t bpm_delta;
};

/// This class manages bpm synchronization across voices.
/// The MIDI spec doesn't have a "increase bpm" command,
/// only "set bpm", so voices have to know the current bpm
/// to increase it and set to the new bpm. This requires
/// bpm updates to be ordered according to elapsed time.
class BpmManager {

public:
  /// Creates a BpmManager with the initial bpm
  BpmManager(int bpm) : initial_bpm(bpm) {}
  /// This function creates a dedicated track to
  /// issue bpm change events.
  void write_bpm_track(MidiCreator &creator);

private:
  int initial_bpm;
  std::vector<BpmChange> changes;
};

} // namespace synthlib

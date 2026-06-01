#pragma once
#include "synthlib/event_consumer.hpp"
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
  /// Creates a BpmManager with the starting bpm.
  /// @param bpm initial bpm
  BpmManager(Bpm bpm) : initial_bpm(bpm) {}
  /// Issues a bpm change in absolute time.
  /// @param beats the song beat in which the change occurs
  /// @param delta the value to add to the burrent bpm
  void change_bpm(unsigned int beats, int delta);
  /// Writes all bpm changes to the track, synchronized
  /// and ordered.
  void write_bpm_track(IEventConsumer &consumer, int track_number);

private:
  Bpm initial_bpm;
  std::vector<BpmChange> changes;
};

} // namespace synthlib

#include "synthlib/bpm_manager.hpp"
#include "synthlib/primitives.hpp"
#include <algorithm>

namespace synthlib {

void BpmManager::change_bpm(uint32_t beats, int delta) {
  changes.push_back((BpmChange){.beats = beats, .bpm_delta = delta});
}

bool compare_bpm(BpmChange &a, BpmChange &b) { return a.beats < b.beats; }

void BpmManager::write_bpm_track(IEventConsumer &consumer, int track_number) {
  std::sort(changes.begin(), changes.end(), compare_bpm);

  consumer.change_track(track_number);

  Bpm bpm = initial_bpm;
  uint32_t beats = 0;
  consumer.set_bpm(bpm);

  for (auto bpm_change : changes) {
    bpm = bpm.add_saturated(bpm_change.bpm_delta);
    consumer.wait_beats(bpm_change.beats - beats);
    consumer.set_bpm(bpm);
    beats = bpm_change.beats;
  }
}

}; // namespace synthlib

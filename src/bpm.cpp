#include "synthlib/bpm.hpp"
#include "synthlib/midi.hpp"
#include <algorithm>

namespace synthlib {

void BpmManager::change_bpm(uint32_t beats, int delta) {
  changes.push_back((BpmChange){.beats = beats, .bpm_delta = delta});
}

bool cmp(BpmChange &a, BpmChange &b) { return a.beats < b.beats; }

void BpmManager::write_bpm_track(MidiCreator &creator, int track_n) {
  std::sort(changes.begin(), changes.end(), cmp);

  creator.goto_track(track_n);

  int bpm = initial_bpm;
  uint32_t beats = 0;
  creator.set_bpm(bpm);

  for (auto bpm_change : changes) {
    bpm += bpm_change.bpm_delta;
    creator.play_pause(bpm_change.beats - beats);
    creator.set_bpm(bpm);
    beats = bpm_change.beats;
  }
}

}; // namespace synthlib

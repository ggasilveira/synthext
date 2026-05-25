#include "synthlib/bpm_manager.hpp"
#include "synthlib/midi_creator.hpp"
#include "synthlib/primitives.hpp"
#include <algorithm>
#include <stdio.h>

namespace synthlib {

void BpmManager::change_bpm(uint32_t beats, int delta) {
  changes.push_back((BpmChange){.beats = beats, .bpm_delta = delta});
}

bool cmp(BpmChange &a, BpmChange &b) { return a.beats < b.beats; }

void BpmManager::write_bpm_track(MidiCreator &creator, int track_n) {
  std::sort(changes.begin(), changes.end(), cmp);

  creator.goto_track(track_n);

  Bpm bpm = initial_bpm;
  uint32_t beats = 0;
  creator.set_bpm(bpm);

  int i = 0;
  for (auto bpm_change : changes) {

    bpm = bpm.add_saturated(bpm_change.bpm_delta);
    creator.play_pause(bpm_change.beats - beats);
    creator.set_bpm(bpm);
    beats = bpm_change.beats;
    printf("writing bpm %d at %d: %d\n", i, bpm_change.beats, bpm);
    ++i;
  }
}

}; // namespace synthlib

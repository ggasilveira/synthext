#include "synthlib/text.hpp"
#include "fluidsynth.h"
#include <iostream>
using std::cerr;
using std::cout;

auto run() -> int {
  fluid_settings_t *settings = nullptr;
  fluid_synth_t *synth = nullptr;
  fluid_audio_driver_t *adriver = nullptr;
  settings = new_fluid_settings();
  if (settings == nullptr) {
    cerr << "failed to create settings\n";
    return 1;
  }
  synth = new_fluid_synth(settings);
  if (synth == nullptr) {
    cerr << "failed to create synth\n";
    return 1;
  }
  if (fluid_synth_sfload(synth, "general.sf2", 1) == -1) {
    cerr << "Failed to load soundfont\n";
    return 1;
  }
  adriver = new_fluid_audio_driver(settings, synth);

  if (adriver == nullptr) {
    cout << "Failed to create the audio driver\n";
    return 1;
  }

  fluid_synth_noteon(synth, 0, 60, 100);
  fgetc(stdin);

  return 0;
}

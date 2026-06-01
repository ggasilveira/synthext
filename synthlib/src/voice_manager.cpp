#include "synthlib/voice_manager.hpp"
#include "synthlib/primitives.hpp"

// NOLINTBEGIN

namespace synthlib {

Volume default_volume(VoiceId voice) {
  return Volume(100 - 20 * (voice.value() % 4));
}
Octave default_octave(VoiceId voice) { return Octave(6 - voice.value() % 4); }

Instrument default_instrument(VoiceId voice) {
  switch (voice.value() % 4) {
  case 0:
    return Instrument(Midi::AcousticGrandPiano);
  case 1:
    return Instrument(Midi::ReedOrgan);
  case 2:
    return Instrument(Midi::Harpsichord);
  default:
    return Instrument(Midi::Clarinet);
  }
}
// NOLINTEND

Bpm VoiceManager::get_bpm() const { return _bpm; }
void VoiceManager::set_bpm(Bpm bpm) { _bpm = bpm; }

Volume VoiceManager::get_volume(VoiceId voice) const {
  try {
    return volume_overrides.at(voice);
  } catch (...) {
    return default_volume(voice);
  }
}

Octave VoiceManager::get_octave(VoiceId voice) const {
  try {
    return octave_overrides.at(voice);
  } catch (...) {
    return default_octave(voice);
  }
}

VoiceParams VoiceManager::get_voice_params(VoiceId voice) const {
  VoiceParams voice_params;
  voice_params.instrument = get_instrument(voice);
  voice_params.volume = get_volume(voice);
  voice_params.octave = get_octave(voice);
  return voice_params;
}

Instrument VoiceManager::get_instrument(VoiceId voice) const {
  try {
    return instrument_overrides.at(voice);
  } catch (...) {
    return default_instrument(voice);
  }
}

void VoiceManager::override_volume(VoiceId voice, Volume volume) {
  volume_overrides[voice] = volume;
}
void VoiceManager::override_octave(VoiceId voice, Octave octave) {
  octave_overrides[voice] = octave;
}
void VoiceManager::override_instrument(VoiceId voice, Instrument instr) {
  instrument_overrides[voice] = instr;
}

void VoiceManager::reset_volume(VoiceId voice) {
  volume_overrides.erase(voice);
}
void VoiceManager::reset_octave(VoiceId voice) {
  octave_overrides.erase(voice);
}
void VoiceManager::reset_instrument(VoiceId voice) {
  instrument_overrides.erase(voice);
}
void VoiceManager::reset_voice(VoiceId voice) {
  reset_volume(voice);
  reset_octave(voice);
  reset_instrument(voice);
}

} // namespace synthlib

// NOLINTBEGIN
#ifdef CFG_TEST
#include "doctest.h"
using namespace synthlib;

TEST_CASE("testing volume overrides") {
  VoiceManager manager;

  CHECK(manager.get_volume(VoiceId(0)).value() == 100);
  CHECK(manager.get_volume(VoiceId(1)).value() == 80);
  CHECK(manager.get_volume(VoiceId(2)).value() == 60);
  CHECK(manager.get_volume(VoiceId(3)).value() == 40);
  CHECK(manager.get_volume(VoiceId(4)).value() == 100);
  CHECK(manager.get_volume(VoiceId(5)).value() == 80);
  CHECK(manager.get_volume(VoiceId(6)).value() == 60);
  CHECK(manager.get_volume(VoiceId(7)).value() == 40);
  CHECK(manager.get_volume(VoiceId(8)).value() == 100);

  manager.override_volume(VoiceId(3), Volume(75));
  CHECK(manager.get_volume(VoiceId(3)).value() == 75);
  manager.reset_volume(VoiceId(3));
  CHECK(manager.get_volume(VoiceId(3)).value() == 40);
}

TEST_CASE("testing octave overrides") {
  VoiceManager manager;

  CHECK(manager.get_octave(VoiceId(0)).value() == 6);
  CHECK(manager.get_octave(VoiceId(1)).value() == 5);
  CHECK(manager.get_octave(VoiceId(2)).value() == 4);
  CHECK(manager.get_octave(VoiceId(3)).value() == 3);
  CHECK(manager.get_octave(VoiceId(4)).value() == 6);
  CHECK(manager.get_octave(VoiceId(5)).value() == 5);
  CHECK(manager.get_octave(VoiceId(6)).value() == 4);
  CHECK(manager.get_octave(VoiceId(7)).value() == 3);
  CHECK(manager.get_octave(VoiceId(8)).value() == 6);

  manager.override_octave(VoiceId(3), Octave(8));
  CHECK(manager.get_octave(VoiceId(3)).value() == 8);
  manager.reset_octave(VoiceId(3));
  CHECK(manager.get_octave(VoiceId(3)).value() == 3);
}

TEST_CASE("testing instrument overrides") {
  VoiceManager manager;

  CHECK(manager.get_instrument(VoiceId(0)).midi() == Midi::AcousticGrandPiano);
  CHECK(manager.get_instrument(VoiceId(1)).midi() == Midi::ReedOrgan);
  CHECK(manager.get_instrument(VoiceId(2)).midi() == Midi::Harpsichord);
  CHECK(manager.get_instrument(VoiceId(3)).midi() == Midi::Clarinet);
  CHECK(manager.get_instrument(VoiceId(4)).midi() == Midi::AcousticGrandPiano);
  CHECK(manager.get_instrument(VoiceId(5)).midi() == Midi::ReedOrgan);
  CHECK(manager.get_instrument(VoiceId(6)).midi() == Midi::Harpsichord);
  CHECK(manager.get_instrument(VoiceId(7)).midi() == Midi::Clarinet);
  CHECK(manager.get_instrument(VoiceId(8)).midi() == Midi::AcousticGrandPiano);

  manager.override_instrument(VoiceId(3), Instrument(Midi::Accordion));
  CHECK(manager.get_instrument(VoiceId(3)).midi() == Midi::Accordion);
  manager.reset_instrument(VoiceId(3));
  CHECK(manager.get_instrument(VoiceId(3)).midi() == Midi::Clarinet);
}
#endif
// NOLINTEND

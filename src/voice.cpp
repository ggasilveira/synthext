#include "synthlib/voice.hpp"
#include "doctest.h"

// NOLINTBEGIN

namespace synthlib {

  Volume default_volume(VoiceId voice) {
    return Volume(100 - 20 * (voice % 4));
  }
  Octave default_octave(VoiceId voice) {
    return Octave(6 - voice % 4);
  }
  
  Instrument default_instrument(VoiceId voice) {
    switch (voice % 4) {
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
  
  Volume VoiceManager::get_volume(VoiceId voice) {
    try {
      return volume_overrides.at(voice);
    } catch (...) {
      return default_volume(voice);
    }
  }
  
  Octave VoiceManager::get_octave(VoiceId voice) {
    try {
      return octave_overrides.at(voice);
    } catch (...) {
      return default_octave(voice);
    }
  }
  
  Instrument VoiceManager::get_instrument(VoiceId voice) {
    try {
      return instrument_overrides.at(voice);
    } catch (...) {
      return default_instrument(voice);
    }
  }

  void VoiceManager::override_volume(VoiceId voice, Volume volume) {
    volume_overrides.emplace(voice, volume);
  }
  void VoiceManager::override_octave(VoiceId voice, Octave octave) {
    octave_overrides.emplace(voice, octave);
  }
  void VoiceManager::override_instrument(VoiceId voice, Instrument instr) {
    instrument_overrides.emplace(voice, instr);
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



  //NOLINTBEGIN

  TEST_CASE("testing volume overrides") {
    VoiceManager manager;

    CHECK(manager.get_volume(0).value() == 100);
    CHECK(manager.get_volume(1).value() == 80);
    CHECK(manager.get_volume(2).value() == 60);
    CHECK(manager.get_volume(3).value() == 40);
    CHECK(manager.get_volume(4).value() == 100);
    CHECK(manager.get_volume(5).value() == 80);
    CHECK(manager.get_volume(6).value() == 60);
    CHECK(manager.get_volume(7).value() == 40);
    CHECK(manager.get_volume(8).value() == 100);

  }


  //NOLINTEND

} // namespace synthlib

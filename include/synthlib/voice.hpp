#pragma once

#include "synthlib/instrument.hpp"
#include "synthlib/octave.hpp"
#include "synthlib/volume.hpp"
#include <map>

namespace synthlib {

using VoiceId = unsigned int;

/// This class is responsible for setting
/// the voices initial parameters. Voices
/// without any overrides will have default
/// parameters assigned to them according
/// to the specification.
class VoiceManager {
public:
  /// Sets the starting BPM
  /// @param bpm the starting bpm
  void set_bpm(uint32_t bpm);
  /// Overrides a voice's initial volume
  /// @param voice the voice number
  /// @param volume the desired volume
  void override_volume(VoiceId voice, Volume volume);

  /// Overrides a voice's initial octave
  /// @param voice the voice number
  /// @param octave the desired octave
  void override_octave(VoiceId voice, Octave octave);

  /// Overrides a voice's initial instrument
  /// @param voice the voice number
  /// @param instr the desired instrument
  void override_instrument(VoiceId voice, Instrument instr);

  /// Resets a voice's initial volume
  /// @param voice the voice number
  void reset_volume(VoiceId voice);

  /// Resets a voice's initial octave
  /// @param voice the voice number
  void reset_octave(VoiceId voice);

  /// Resets a voice's initial instrument
  /// @param voice the voice number
  void reset_instrument(VoiceId voice);

  /// Resets all parameters of the voice
  /// @param voice the voice number
  void reset_voice(VoiceId voice);

  /// Gets the voice's initial volume
  /// @param voice the voice number
  /// @return The voice's initial volume
  Volume get_volume(VoiceId voice) const;

  /// Gets the voice's initial octave
  /// @param voice the voice number
  /// @return The voice's initial octave
  Octave get_octave(VoiceId voice) const;

  /// Gets the voice's initial instrument
  /// @param voice the voice number
  /// @return The voice's initial instrument
  Instrument get_instrument(VoiceId voice) const;

  /// Gets the starting bpm
  /// @return The starting bpm
  uint32_t get_bpm() const;

private:
  std::map<VoiceId, Volume> volume_overrides;
  std::map<VoiceId, Octave> octave_overrides;
  std::map<VoiceId, Instrument> instrument_overrides;
  uint32_t bpm = 120;
};

} // namespace synthlib

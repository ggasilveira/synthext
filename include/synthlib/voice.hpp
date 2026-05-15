#pragma once

#include "synthlib/instrument.hpp"
#include <map>
#include <cstdint>
#include <stdexcept>

namespace synthlib {

  /// A volume value
  class Volume {
  public:
    /// Minimum volume value
    static const uint8_t MIN = 0;
    /// Maximum volume value
    static const uint8_t MAX = 127;

    /// Constructs a Volume if argument is in the range [MIN; MAX]
    /// @param volume Volume value
    /// @return A Volume object
    /// @throws std::invalid_argument if volume out of range
    Volume(uint8_t volume): _vol(volume) {
      if (_vol > MAX) {
	throw std::invalid_argument("exceeded maximum volume");
      }
    }
    /// Gets the volume value
    /// @return The volume value
    [[nodiscard]]
    uint8_t value() {
      return _vol;
    }
  private:
    uint8_t _vol;
  };

  /// An octave value
  class Octave {
  public:
    /// Miminum octave value
    static const uint8_t MIN = 0;
    /// Maximum octave value
    static const uint8_t MAX = 9;

    /// Constructs an Octave if argument is in the range [MIN; MAX]
    /// @param octave Octave value
    /// @return An Octave object
    /// @throws std::invalid_argument if octave out of range
    Octave(uint8_t octave): _octave(octave) {
      if (_octave > MAX) {
	throw std::invalid_argument("exceeded maximum octave");
      }
    }
    /// Gets the octave value
    /// @return The octave value
    [[nodiscard]]
    uint8_t value() {
      return _octave;
    }

  private:
    uint8_t _octave;
  };

  using VoiceId = unsigned int;

  /// This class is responsible for setting
  /// the voices initial parameters. Voices
  /// without any overrides will have default
  /// parameters assigned to them according
  /// to the specification.
  class VoiceManager {
  public:

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
    Volume get_volume(VoiceId voice);

    /// Gets the voice's initial octave
    /// @param voice the voice number
    /// @return The voice's initial octave
    Octave get_octave(VoiceId voice);

    /// Gets the voice's initial instrument
    /// @param voice the voice number
    /// @return The voice's initial instrument
    Instrument get_instrument(VoiceId voice);
    
  private:
    std::map<VoiceId, Volume> volume_overrides;
    std::map<VoiceId, Octave> octave_overrides;
    std::map<VoiceId, Instrument> instrument_overrides;
    
  };

} // namespace synthlib

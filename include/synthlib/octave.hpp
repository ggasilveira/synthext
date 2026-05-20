#pragma once

#include <cstdint>
#include <stdexcept>

namespace synthlib {

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
  Octave(uint8_t octave) : _octave(octave) {
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

} // namespace synthlib

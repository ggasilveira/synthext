#pragma once
#include <cstdint>

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
  Volume(uint8_t volume) : _vol(volume) {
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

} // namespace synthlib

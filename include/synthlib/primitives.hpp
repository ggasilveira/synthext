#pragma once
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace synthlib {
static constexpr int VOICE_MIN = 0;
static constexpr int VOICE_MAX = 15;
static constexpr int VOICE_INITIAL = 0;

static constexpr int CHANNEL_MIN = 0;
static constexpr int CHANNEL_MAX = 15;
static constexpr int CHANNEL_INITIAL = 0;

static constexpr int BPM_MIN = 0;
static constexpr int BPM_MAX = 400;
static constexpr int BPM_INITIAL = 120;

static constexpr int OCTAVE_MIN = 0;
static constexpr int OCTAVE_MAX = 9;
static constexpr int OCTAVE_INITIAL = 0;

static constexpr int VOLUME_MIN = 0;
static constexpr int VOLUME_MAX = 127;
static constexpr int VOLUME_INITIAL = 0;

/// Generic class for bounded values
template <typename T, T min, T max, T initial> class Bounded {
public:
  static constexpr T MIN = min;
  static constexpr T MAX = max;
  static constexpr T DEFAULT = initial;

  Bounded() : _value(DEFAULT) {}
  /// Validates and creates the bounded object
  Bounded(T value) : _value(value) {
    if (value < MIN || value > MAX) {
      throw std::invalid_argument("value out of range");
    }
  }

  /// add a value and clamp result inside bounds
  Bounded<T, min, max, initial> add_saturated(T value) {
    T newval = _value + value;
    if (newval < MIN) {
      return Bounded<T, min, max, initial>(MIN);
    }
    if (newval > MAX) {
      return Bounded<T, min, max, initial>(MAX);
    }
    return Bounded<T, min, max, initial>(newval);
  }

  /// subtract a value and clamp result inside bounds
  Bounded<T, min, max, initial> sub_saturated(T value) {
    T newval = _value - value;
    if (newval < MIN) {
      return Bounded(MIN);
    }
    if (newval > MAX) {
      return Bounded(MAX);
    }
    return Bounded(newval);
  }

  /// Returns the inner value
  [[nodiscard]]
  T value() const {
    return _value;
  }

  friend bool operator<(const Bounded<T, min, max, initial> &lhs,
                        const Bounded<T, min, max, initial> &rhs) {
    return lhs.value() < rhs.value();
  }
  friend bool operator>(const Bounded<T, min, max, initial> &lhs,
                        const Bounded<T, min, max, initial> &rhs) {
    return lhs.value() > rhs.value();
  }
  friend bool operator<=(const Bounded<T, min, max, initial> &lhs,
                         const Bounded<T, min, max, initial> &rhs) {
    return lhs.value() <= rhs.value();
  }
  friend bool operator>=(const Bounded<T, min, max, initial> &lhs,
                         const Bounded<T, min, max, initial> &rhs) {
    return lhs.value() >= rhs.value();
  }
  friend bool operator==(const Bounded<T, min, max, initial> &lhs,
                         const Bounded<T, min, max, initial> &rhs) {
    return lhs.value() == rhs.value();
  }
  friend bool operator!=(const Bounded<T, min, max, initial> &lhs,
                         const Bounded<T, min, max, initial> &rhs) {
    return lhs.value() != rhs.value();
  }

private:
  T _value;
};

/// A range-validated bpm value
using Bpm = Bounded<int, BPM_MIN, BPM_MAX, BPM_INITIAL>;
// class Bpm : public Bounded<int, BPM_MIN, BPM_MAX, BPM_INITIAL> {};

/// A range-validated voice identifier
using VoiceId = Bounded<int, VOICE_MIN, VOICE_MAX, VOICE_INITIAL>;
// class VoiceId : public Bounded<int, VOICE_MIN, VOICE_MAX, VOICE_INITIAL> {};

/// A range-validated octave value
using Octave = Bounded<int, OCTAVE_MIN, OCTAVE_MAX, OCTAVE_INITIAL>;
// class Octave : public Bounded<int, OCTAVE_MIN, OCTAVE_MAX, OCTAVE_INITIAL>
// {};

/// A range-validated volume value
using Volume = Bounded<int, VOLUME_MIN, VOLUME_MAX, VOLUME_INITIAL>;
// class Volume : public Bounded<int, VOLUME_MIN, VOLUME_MAX, VOLUME_INITIAL>
// {};

/// A range-validated channel value
using Channel = Bounded<int, CHANNEL_MIN, CHANNEL_MAX, CHANNEL_INITIAL>;

/// Class representing musical notes
enum class Note : uint8_t {
  C = 0,
  Cs = 1,
  Db = 1,
  D = 2,
  Ds = 3,
  Eb = 3,
  E = 4,
  F = 5,
  Fs = 6,
  Gb = 6,
  G = 7,
  Gs = 8,
  Ab = 8,
  A = 9,
  As = 10,
  Bb = 10,
  B = 11,
};

} // namespace synthlib

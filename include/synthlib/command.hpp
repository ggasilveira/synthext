#pragma once

#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include <cstdint>
#include <variant>

namespace synthlib {
/// Types of synthext commands
enum class CommandKind : uint8_t {
  PlayNote,
  Pause,
  PauseOrRepeat,
  Delay,
  IncreaseBpm,
  DecreaseBpm,
  IncreaseOctave,
  DecreaseOctave,
  DoubleVolume,
  ChangeInstrument,
  IncreaseInstrument
};

/// A synthext command. Some commands have associated values.
class Command {
private:
  CommandKind _kind;
  std::variant<Note, uint32_t, Instrument> value;

public:
  /// The default command is pause or repeat, which is issued for
  /// the remaining characters
  Command() : _kind(CommandKind::PauseOrRepeat), value((uint32_t)0) {}

  /// Create a note playing command
  Command(Note note) : _kind(CommandKind::PlayNote), value(note) {}
  /// Create an instrument change command
  Command(Instrument instr)
      : _kind(CommandKind::ChangeInstrument), value(instr) {}
  /// Create a command of type with associated integer value
  Command(CommandKind kind, uint32_t value) : _kind(kind), value(value) {}
  /// Create a command of type
  Command(CommandKind kind) : _kind(kind), value((uint32_t)0) {}

  /// The type of command
  CommandKind kind() const { return _kind; }
  /// The note of the PlayNote command type
  Note note() const { return std::get<Note>(value); }
  /// Value used for certain command types
  uint32_t amount() const { return std::get<uint32_t>(value); }
  /// Instrument to change to
  Instrument instrument() const { return std::get<Instrument>(value); }
};

bool operator==(const Command &lhs, const Command &rhs);
} // namespace synthlib

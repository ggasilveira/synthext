#pragma once

#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include <cstdint>
#include <stdexcept>
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
  Note _note = Note::A;
  int _amount = 0;
  Instrument _instr = Instrument(Midi::AcousticGrandPiano);

public:
  /// The default command is pause or repeat, which is issued for
  /// the remaining characters
  Command() : _kind(CommandKind::PauseOrRepeat) {}

  /// Create a note playing command.
  /// @param note the note to play
  Command(Note note) : _kind(CommandKind::PlayNote), _note(note) {}
  /// Create an instrument change command.
  /// @param instr the instrument to change to
  Command(Instrument instr)
      : _kind(CommandKind::ChangeInstrument), _instr(instr) {}
  /// Create a command of type with associated integer value.
  /// @param kind the type of command to create
  /// @param value the associated value
  Command(CommandKind kind, int value) : _kind(kind), _amount(value) {}
  /// Create a command of the given type.
  /// @param kind the type of command to create
  Command(CommandKind kind) : _kind(kind) {}

  /// Return the type of this command.
  /// @return this command's type
  CommandKind kind() const { return _kind; }
  /// Return the note if this command is of type PlayNote.
  /// @return this command's note
  /// @throws std::runtime_error if the command's type is not PlayNote
  Note note() const {
    if (_kind != CommandKind::PlayNote) {
      throw std::runtime_error("command kind != PlayNote");
    }
    return _note;
  }
  /// Value used for certain command types
  int amount() const { return _amount; }
  /// Return the target instrument if this command is of type ChangeInstrument
  /// @return this command's instrument
  /// @throws std::runtime_error if this command's type is not ChangeInstrument
  Instrument instrument() const {
    if (_kind != CommandKind::ChangeInstrument) {
      throw std::runtime_error("command kind != ChangeInstrument");
    }
    return _instr;
  }
};

bool operator==(const Command &lhs, const Command &rhs);
} // namespace synthlib

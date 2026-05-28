#pragma once

#include "synthlib/bpm_manager.hpp"
#include "synthlib/event_consumer.hpp"
#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include <cstdint>
#include <optional>
#include <stdexcept>

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

/// Context passed for command execution
class CommandContext {
public:
  CommandContext(IEventConsumer &consumer, BpmManager &bpm_manager,
                 Channel channel, Instrument instr, Octave octave,
                 Volume volume)
      : _consumer(consumer), _bpm_manager(bpm_manager), _channel(channel),
        instrument(instr), octave(octave), volume(volume),
        last_note(std::nullopt) {}

  IEventConsumer &consumer() { return _consumer; }
  BpmManager &bpm_manager() { return _bpm_manager; }
  Channel channel() { return _channel; }

  Instrument instrument;
  Octave octave;
  Volume volume;
  std::optional<Note> last_note;

private:
  /// The event consumer
  IEventConsumer &_consumer;
  BpmManager &_bpm_manager;
  Channel _channel;
};

/// Interface for commands to execute different actions
class ICommand {
public:
  virtual ~ICommand() = default;
  /// Execute the command using the execution context.
  /// @param ctx the command execution context
  virtual void execute(CommandContext &ctx);
  /// Returns the number of beats taken to execute the command.
  /// @return number of beats taken by the command
  virtual unsigned int beats_taken();

  /// Returns true if this command was a note playing command
  virtual bool plays_note() { return false; }
};

class Pause : ICommand {

  void execute(CommandContext &ctx) override;
  unsigned int beats_taken() override;
  unsigned int _beats;

public:
  Pause(int beats) : _beats(beats) {}
};

class PauseOrRepeat : ICommand {
  void execute(CommandContext &ctx) override;
  unsigned int beats_taken() override;
};

class PlayNote : ICommand {
  void execute(CommandContext &ctx) override;
  unsigned int beats_taken() override;
  Note _note;

public:
  PlayNote(Note note) : _note(note) {}
};
class ChangeInstrument : ICommand {
  void execute(CommandContext &ctx) override;
  unsigned int beats_taken() override;
  Instrument _instr;

public:
  ChangeInstrument(Instrument instr) : _instr(instr) {}
};
class AddOctave : ICommand {
  void execute(CommandContext &ctx) override;
  unsigned int beats_taken() override;
  int _amount;

public:
  AddOctave(int amount) : _amount(amount) {}
};

class AddBpm : ICommand {
  void execute(CommandContext &ctx) override;
  unsigned int beats_taken() override;
  int _amount;

public:
  AddBpm(int amount) : _amount(amount) {}
};

class DoubleVolume : ICommand {
  void execute(CommandContext &ctx) override;
  unsigned int beats_taken() override;
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

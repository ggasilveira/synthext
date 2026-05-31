#pragma once

#include "synthlib/bpm_manager.hpp"
#include "synthlib/event_consumer.hpp"
#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include "synthlib/voice_manager.hpp"
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

/// Context passed for command execution.
/// This context is supposed to live for ONE voice line only,
/// so you need to recreate the context for every line to be processed.
class CommandContext {
public:
  /// Creates a new CommandContext to process a SINGLE voice line.
  /// @param consumer the event consumer
  /// @param bpm_manager the bpm manager to send bpm change events
  /// @param channel the voice channel
  /// @param voice_params the initial voice parameters
  CommandContext(IEventConsumer &consumer, BpmManager &bpm_manager,
                 Channel channel, VoiceParams voice_params)
      : _consumer(consumer), _bpm_manager(bpm_manager), _channel(channel),
        _initial(voice_params), _current(voice_params),
        _last_note(std::nullopt) {}

  IEventConsumer &consumer() { return _consumer; }
  BpmManager &bpm_manager() { return _bpm_manager; }

  Channel channel() const { return _channel; }

  Instrument instrument() const { return _current.instrument; }
  Instrument initial_instrument() const { return _initial.instrument; }

  Octave octave() const { return _current.octave; }
  Octave initial_octave() const { return _initial.octave; }

  Volume volume() const { return _current.volume; }
  Volume initial_volume() const { return _initial.volume; }

  std::optional<Note> last_note() const { return _last_note; }
  unsigned int current_beat() const { return _current_beat; }

  void set_instrument(Instrument instrument) {
    _current.instrument = instrument;
  }
  void set_octave(Octave octave) { _current.octave = octave; }
  void set_volume(Volume volume) { _current.volume = volume; }

  void set_last_note(std::optional<Note> note) { _last_note = note; }
  void advance_beats(unsigned int beats) { _current_beat += beats; }

private:
  /// The event consumer
  IEventConsumer &_consumer;
  BpmManager &_bpm_manager;
  Channel _channel;
  VoiceParams _initial;
  VoiceParams _current;
  std::optional<Note> _last_note;
  unsigned int _current_beat = 0;
};

/// Interface for commands to execute different actions
class ICommand {
public:
  virtual ~ICommand() {}

  /// Execute the command using the execution context.
  /// @param ctx the command execution context
  void execute(CommandContext &ctx) {
    _execute(ctx);
    ctx.set_last_note(note_played(ctx));
    ctx.advance_beats(beats_taken());
  }
  /// Returns the number of beats taken to execute the command.
  /// @return number of beats taken by the command
  virtual unsigned int beats_taken() const { return 0; }

  /// Returns true if this command was a note playing command
  virtual std::optional<Note> note_played(const CommandContext &ctx) const {
    return std::nullopt;
  }

protected:
  /// Execute the command using the execution context.
  /// This method is the one subclasses must override.
  /// @param ctx the command execution context
  virtual void _execute(CommandContext &ctx) = 0;
};

/// Pauses the execution for n beats
class Pause : public ICommand {
public:
  Pause(int beats) : _beats(beats) {}
  unsigned int beats_taken() const override;

private:
  unsigned int _beats;
  void _execute(CommandContext &ctx) override;
};

/// If the last command was a note, repeat the note,
/// else pause the execution for one beat.
class PauseOrRepeat : public ICommand {
private:
  void _execute(CommandContext &ctx) override;
  unsigned int beats_taken() const override;
  std::optional<Note> note_played(const CommandContext &ctx) const override;
};

/// Plays a note for one beat.
class PlayNote : public ICommand {
public:
  PlayNote(Note note) : _note(note) {}
  unsigned int beats_taken() const override;

private:
  Note _note;
  void _execute(CommandContext &ctx) override;
  std::optional<Note> note_played(const CommandContext &ctx) const override;
};

/// Changes the voice current instrument.
class ChangeInstrument : public ICommand {
public:
  ChangeInstrument(Instrument instr) : _instr(instr) {}

private:
  Instrument _instr;
  void _execute(CommandContext &ctx) override;
};

/// Adds a value to the MIDI value of the current instrument,
/// returning to the default if exceeding the maximum instrument.
class AddInstrument : public ICommand {
public:
  AddInstrument(int amount) : _amount(amount) {}

private:
  int _amount;
  void _execute(CommandContext &ctx) override;
};

/// Adds a value to the current voice octave,
/// returning to the default if exceeding the maximum
/// octave.
class AddOctave : public ICommand {
public:
  AddOctave(int amount) : _amount(amount) {}

private:
  int _amount;
  void _execute(CommandContext &ctx) override;
};

/// Adds a value to the global BPM.
class AddBpm : public ICommand {
public:
  AddBpm(int amount) : _amount(amount) {}

private:
  int _amount;
  void _execute(CommandContext &ctx) override;
};

/// Doubles the voice volume, saturating if the maximum
/// was reached.
class DoubleVolume : public ICommand {
private:
  void _execute(CommandContext &ctx) override;
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

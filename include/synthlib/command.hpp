#pragma once
#include "synthlib/midi.hpp"
#include "synthlib/note.hpp"
#include "synthlib/player.hpp"
#include <memory>

namespace synthlib {

class ICommand {
  // protected:
  //  ICommand() = default;

public:
  virtual ~ICommand() = default;
  [[nodiscard]]
  virtual std::unique_ptr<ICommand> clone() const = 0;
  // ICommand(const ICommand &) = delete;
  // ICommand(const ICommand &&) = delete;
  // auto operator=(const ICommand &) -> ICommand & = delete;
  // auto operator=(const ICommand &&) -> ICommand & = delete;

  /// Command subclasses implement this function to
  /// manipulate the player
  virtual void execute(synthlib::IPlayer &player) = 0;
};

class CommandEmpty : public ICommand {
public:
  void execute(IPlayer &player) override {}
};

/// Plays the given note
class CommandNote : public ICommand {
  Note note;

public:
  CommandNote(Note note) : note(note) {}

  void execute(synthlib::IPlayer &player) override;
  [[nodiscard]]
  std::unique_ptr<ICommand> clone() const override {
    return std::make_unique<CommandNote>(*this);
  }
};

/// Changes to the given MIDI instrument
class CommandChangeInstrument : public ICommand {
  Instrument midi;

public:
  CommandChangeInstrument(Instrument midi) : midi(midi) {}

  void execute(synthlib::IPlayer &player) override;
  [[nodiscard]]
  std::unique_ptr<ICommand> clone() const override {
    return std::make_unique<CommandChangeInstrument>(*this);
  }
};

/// Increases or decreases the current MIDI instrument by `amount`
class CommandMoveInstrument : public ICommand {
  int amount;

public:
  CommandMoveInstrument(int amount): amount(amount) {}

  void execute(synthlib::IPlayer &player) override;
  [[nodiscard]]
  std::unique_ptr<ICommand> clone() const override {
    return std::make_unique<CommandMoveInstrument>(*this);
  }
};

/// Plays silence
class CommandSilence : public ICommand {
public:
  void execute(synthlib::IPlayer &player) override;
  [[nodiscard]]
  std::unique_ptr<ICommand> clone() const override {
    return std::make_unique<CommandSilence>(*this);
  }
};

class CommandDoubleVolume : public ICommand {
public:
  void execute(synthlib::IPlayer &player) override;

  [[nodiscard]]
  std::unique_ptr<ICommand> clone() const override {
    return std::make_unique<CommandDoubleVolume>(*this);
  }
};

class CommandAddOctave : public ICommand {
  int amount;

public:
  CommandAddOctave(int amount) : amount(amount) {}
  void execute(IPlayer &player) override;
  [[nodiscard]]
  std::unique_ptr<ICommand> clone() const override {
    return std::make_unique<CommandAddOctave>(*this);
  }
};

class CommandIncreaseBpm : public ICommand {
  int amount;

public:
  CommandIncreaseBpm(int amount) : amount(amount) {}
  void execute(IPlayer &player) override;
  [[nodiscard]]
  std::unique_ptr<ICommand> clone() const override {
    return std::make_unique<CommandIncreaseBpm>(*this);
  }
  
};

} // namespace synthlib

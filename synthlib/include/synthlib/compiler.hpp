#pragma once
#include "synthlib/command.hpp"
#include "synthlib/event_consumer.hpp"
#include "synthlib/voice_manager.hpp"
#include <array>
#include <string>
#include <vector>

namespace synthlib {

/// A Compilation Error
class CompilerError : std::exception {

public:
  /// The type of compilation error
  enum class Kind : uint8_t {
    UnclosedDelay,
    InvalidDelay,

  };
  /// Creates a new error with the given kind on the given line
  CompilerError(Kind kind, int line) : _kind(kind), _line(line) {
    _msg = gen_msg();
  }
  const char *what() const noexcept override { return _msg.c_str(); }

  /// @return the type of error
  Kind kind() const { return _kind; }
  /// @return the line of the error
  int line() const { return _line; }

private:
  Kind _kind;
  int _line;
  std::string _msg;

  std::string gen_msg() const;
};

/// This class compiles Synthext text files into MIDI files.
/// The compiler configuration is done through the `VoiceParams`
/// class.
class Compiler {
public:
  using Mapping = std::array<Command, 256>;
  using Voiceline = std::vector<Command>;

  /// Creates a new compiler.
  Compiler();

  /// Compiles Synthext source into a MIDI file.
  /// @param voice_params the voices configuration
  /// @param source the Synthext language source text
  /// @return the MIDI file as a byte vector
  /// @throw CompilerError if there was any compilation error
  void compile(IEventConsumer &consumer, const VoiceManager &voice_params,
               std::string source) const;

  /// Compiles a Synthext voice into a sequece of commands
  /// @param line the voice line
  /// @return A compiled vector of commands
  Voiceline compile_line(const std::string &line) const;

private:
  /// The mapping from character -> Command.
  Mapping map;
};

} // namespace synthlib

#pragma once
#include "synthlib/command.hpp"
#include "synthlib/voice_manager.hpp"
#include <array>
#include <string>
#include <vector>

namespace synthlib {

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
  /// @param source the Synthext language source text
  /// @return the MIDI file as a byte vector
  std::vector<uint8_t> compile(const VoiceManager &voice_params,
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

#include "synthlib/compiler.hpp"
#include "doctest.h"
#include "synthlib/midi.hpp"
#include <array>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <variant>
#include <vector>

using std::vector, std::string_view, std::variant;

namespace synthlib {
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

class Command {
private:
  CommandKind _kind;
  variant<Note, uint32_t, Instrument> value;

public:
  Command() : _kind(CommandKind::PauseOrRepeat), value((uint32_t)0) {}
  Command(Note note) : _kind(CommandKind::PlayNote), value(note) {}
  Command(Instrument instr)
      : _kind(CommandKind::ChangeInstrument), value(instr) {}
  Command(CommandKind kind, uint32_t value) : _kind(kind), value(value) {}
  Command(CommandKind kind) : _kind(kind), value((uint32_t)0) {}

  CommandKind kind() const { return _kind; }
  Note note() const { return std::get<Note>(value); }
  uint32_t amount() const { return std::get<uint32_t>(value); }
  Instrument instrument() const { return std::get<Instrument>(value); }
  void debug() {
    switch (_kind) {
    case CommandKind::ChangeInstrument:
      std::cout << "CH_INSTRUMENT(" << std::get<Instrument>(value).name()
                << ")";
      break;
    case CommandKind::PlayNote:
      std::cout << "NOTE";
      break;
    case CommandKind::Pause:
      std::cout << "PAUSE";
      break;
    case CommandKind::PauseOrRepeat:
      std::cout << "PAUSE_REP";
      break;
    case CommandKind::Delay:
      std::cout << "DELAY";
      break;
    case CommandKind::IncreaseBpm:
      std::cout << "INC_BPM";
      break;
    case CommandKind::DecreaseBpm:
      std::cout << "DEC_BPM";
      break;
    case CommandKind::IncreaseOctave:
      std::cout << "INC_OCTAVE";
      break;
    case CommandKind::DecreaseOctave:
      std::cout << "DEC_OCTAVE";
      break;
    case CommandKind::DoubleVolume:
      std::cout << "DOUBLE_VOL";
      break;
    case CommandKind::IncreaseInstrument:
      std::cout << "INC_INSTR";
      break;
    }
  }
};

bool operator==(const Command &lhs, const Command &rhs) {
  if (lhs.kind() != rhs.kind()) {
    return false;
  }
  switch (lhs.kind()) {
  case CommandKind::PlayNote:
    return lhs.note() == rhs.note();
    break;
  case CommandKind::Delay:
  case CommandKind::IncreaseInstrument:
    return lhs.amount() == rhs.amount();
    break;
  case CommandKind::ChangeInstrument:
    return lhs.instrument().midi() == rhs.instrument().midi();
  default:
    return true;
  }
}

using cursor = string_view::size_type;

using mapping = std::array<Command, 256>;
using voiceline = std::vector<Command>;

static const uint8_t Mb = '+';
static const char *Mb_s = "+";

// Mapping for synthext language
// We replace all occurrences of "Mb", which is the only
// multi-letter token to "+", which is an unused character
// so we can use an array to map characters to commands.
mapping build_map() {
  mapping map{};

  // note commands
  map['A'] = Command(Note::A);
  map['B'] = Command(Note::B);
  map['C'] = Command(Note::C);
  map['D'] = Command(Note::D);
  map['E'] = Command(Note::E);
  map['F'] = Command(Note::F);
  map['G'] = Command(Note::G);
  map['H'] = Command(Note::Bb);

  map[Mb] = Command(Note::Eb);

  // pause commands
  for (auto c : "abcdefgh") {
    map.at(c) = Command(CommandKind::Pause);
  }

  // volume commands
  map[' '] = Command(CommandKind::DoubleVolume);

  // instrument commands
  map['!'] = Command(Instrument(Midi::Harmonica));
  map[';'] = Command(Instrument(Midi::TubularBells));
  map[','] = Command(Instrument(Midi::ChurchOrgan));
  for (auto c : "13579") {
    map.at(c) = Command(Instrument(Midi::TubularBells));
  }
  for (auto c : "02468") {
    map.at(c) = Command(CommandKind::IncreaseInstrument, c - '0');
  }

  // octave commands
  map['?'] = Command(CommandKind::IncreaseOctave);
  map['V'] = Command(CommandKind::DecreaseOctave);

  // bpm commands
  map['<'] = Command(CommandKind::DecreaseBpm);
  map['>'] = Command(CommandKind::IncreaseBpm);

  return map;
}

/// Parses a delay clause "[<number>]"
int parse_delay(std::string_view delay) {
  int out = 0;
  std::cout << "DELAY: " << delay << "\n";
  for (auto c : delay) {
    if (c >= '0' && c <= '9') {
      out *= 10;
      out += c - '0';
    } else {
      throw std::invalid_argument("delay specifier is not a number");
    }
  }
  return out;
}

/// Compiles a single line into a voice
voiceline compile_line(const std::string &line, mapping const &map) {
  // replace all occurrences of "Mb" -> "+"
  std::regex Mb_pattern("Mb");
  auto linefix = std::regex_replace(line, Mb_pattern, Mb_s);

  std::vector<Command> comms;
  CommandKind last = CommandKind::Pause;
  char last_char = 0;
  string_view::size_type cursor = 0;

  if (linefix.at(0) == '[') {
    cursor = linefix.find(']');
    if (cursor == string_view::npos) {
      throw std::invalid_argument("unclosed delay specifier");
    }
    int delay = parse_delay(linefix.substr(1, cursor - 1));
    comms.emplace_back(CommandKind::Delay, delay);
    ++cursor;
  }

  for (auto c : linefix.substr(cursor)) {
    comms.push_back(map.at(c));
  }

  return comms;
}

std::vector<std::string> get_lines(std::string &source) {
  std::vector<std::string> lines;
  int line_start = 0;
  int line_len = 0;

  for (char c : source) {
    if (c == '\n') {
      lines.push_back(source.substr(line_start, line_len));
      line_start += line_len + 1;
      line_len = 0;
    } else {
      ++line_len;
    }
  }

  lines.push_back(source.substr(line_start, line_len));
  return lines;
}

// std::vector<uint8_t> create_midi(const VoiceManager params,
//                                  std::vector<voiceline> voices) {
//   MidiCreator creator;
//   for (int i = 0; i < voices.size(); ++i) {
//     creator.goto_track(i);
//     if (i == 0) {
//       creator.set_bpm(params.get_bpm());
//     }
//     for (auto command : voices.at(i)) {
//       Command last_comm;
//       switch (command.kind()) {
//       case CommandKind::Delay:
//         creator.play_pause(command.amount());
//         break;
//       case CommandKind::Pause:
//         creator.play_pause();
//         break;
//       case CommandKind::PauseOrRepeat:
//         if (last_comm.kind() == CommandKind::PlayNote) {
//           creator.play_note()
//         }
//       }
//     }
//   }
// }

std::vector<uint8_t> compile(const VoiceManager &params, std::string source) {

  std::vector<std::string> lines = get_lines(source);
  std::vector<voiceline> voices;
  voices.reserve(lines.size());

  mapping map = build_map();

  for (auto &line : lines) {
    if (line.empty()) {
      continue;
    }
    voices.push_back(compile_line(line, map));
  }
}
// NOLINTBEGIN

TEST_CASE("Line separating") {
  std::string s("line 1\nline 2 \n\n line 4");
  auto lines = get_lines(s);
  CHECK(lines.size() == 4);
  CHECK(lines.at(0) == "line 1");
  CHECK(lines.at(1) == "line 2 ");
  CHECK(lines.at(2) == "");
  CHECK(lines.at(3) == " line 4");

  s = "line 1";
  lines = get_lines(s);
  CHECK(lines.size() == 1);
  CHECK(lines.at(0) == "line 1");

  s = "";
  lines = get_lines(s);
  CHECK(lines.size() == 1);
  CHECK(lines.at(0) == "");

  s = "\n";
  lines = get_lines(s);
  CHECK(lines.size() == 2);
  CHECK(lines.at(0) == "");
  CHECK(lines.at(1) == "");
}

TEST_CASE("voice parsing notes") {
  std::string s = "ABCDEFGHMb";
  std::vector expected = {
      Command(Note::A), Command(Note::B),  Command(Note::C),
      Command(Note::D), Command(Note::E),  Command(Note::F),
      Command(Note::G), Command(Note::Bb), Command(Note::Eb),
  };
  mapping map = build_map();
  auto voice = compile_line(s, map);
  CHECK(voice.size() == 9);
  for (int i = 0; i < 9; ++i) {
    CHECK(voice.at(i) == expected.at(i));
  }
}

// NOLINTEND

} // namespace synthlib

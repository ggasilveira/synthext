#include "synthlib/compiler.hpp"
#include "synthlib/bpm_manager.hpp"
#include "synthlib/command.hpp"
#include "synthlib/midi_creator.hpp"
#include <array>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <variant>
#include <vector>

using std::vector, std::string_view, std::variant;

namespace synthlib {

using cursor = string_view::size_type;

using mapping = std::array<Command, 256>;
using voiceline = std::vector<Command>;

static const uint8_t Mb = '+';
static const char *Mb_s = "+";

// Mapping for synthext language
// We replace all occurrences of "Mb", which is the only
// multi-letter token with "+", which is an unused character
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
  map['<'] = Command(CommandKind::DecreaseBpm, 10);
  map['>'] = Command(CommandKind::IncreaseBpm, 10);

  return map;
}

/// Parses a delay clause "[<number>]"
static int parse_delay(std::string_view delay) {
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
static voiceline compile_line(const std::string &line, mapping const &map) {

  // replace all occurrences of "Mb" with "+"
  std::regex Mb_pattern("Mb");
  auto linefix = std::regex_replace(line, Mb_pattern, Mb_s);

  std::vector<Command> comms;
  CommandKind last = CommandKind::Pause;
  char last_char = 0;
  string_view::size_type cursor = 0;

  // parse delay specifier
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

int voice2track(int voice) { return voice + 1; }

void create_midi_voice(MidiCreator &creator, const VoiceManager &params,
                       BpmManager &bpm_man, voiceline &voice, VoiceId voice_id,
                       int track_num) {
  // we need to keep count of elapsed beats to send bpm changes
  // to the bpm manager
  uint32_t elapsed = 0;
  // last command for pause or repeat
  Command last;
  Channel channel(voice_id.value());
  // get the initial parameters
  Instrument instr = params.get_instrument(voice_id);
  Octave octave = params.get_octave(voice_id);
  Volume volume = params.get_volume(voice_id);

  creator.goto_track(track_num);

  // creator.pause_ticks(1);
  creator.change_instrument(channel, instr);
  std::cout << "changed instr to " << instr.name() << "\n";

  for (auto command : voice) {
    switch (command.kind()) {
    case CommandKind::Delay:
      creator.pause_beats(command.amount());
      elapsed += command.amount();
      break;
    case CommandKind::Pause:
      creator.pause_beats();
      elapsed += 1;
      break;
    case CommandKind::PauseOrRepeat:
      if (last.kind() == CommandKind::PlayNote) {
        creator.play_note(channel, last.note(), octave, volume);
      } else {
        creator.pause_beats();
      }
      elapsed += 1;
      break;
    case CommandKind::PlayNote:
      creator.play_note(channel, command.note(), octave, volume);
      elapsed += 1;
      break;
    case CommandKind::ChangeInstrument:
      instr = command.instrument();
      creator.change_instrument(channel, instr);
      break;
    case CommandKind::IncreaseInstrument:
      instr = Instrument(instr.to_int() + command.amount());
      creator.change_instrument(channel, instr);
      break;
    case CommandKind::IncreaseOctave:
      if (octave.value() < Octave::MAX) {
        octave = Octave(octave.value() + 1);
      } else {
        octave = params.get_octave(voice_id);
      }
      break;
    case CommandKind::DecreaseOctave:
      if (octave.value() > Octave::MIN) {
        octave = Octave(octave.value() - 1);
      } else {
        octave = params.get_octave(voice_id);
      }
      break;
    case CommandKind::IncreaseBpm:
      bpm_man.change_bpm(elapsed, command.amount());
      break;
    case CommandKind::DecreaseBpm:
      bpm_man.change_bpm(elapsed, -command.amount());
      break;
    case CommandKind::DoubleVolume:
      try {
        volume = Volume(volume.value() * 2);
      } catch (...) {
        volume = Volume(Volume::MAX);
      }
      break;
    }
    last = command;
  }
}

std::vector<uint8_t> create_midi(const VoiceManager &params,
                                 std::vector<voiceline> voices) {
  MidiCreator creator(1);
  BpmManager bpm_man(params.get_bpm());
  if (voices.size() > (VoiceId::MAX + 1)) {
    voices.resize(VoiceId::MAX + 1);
  }
  for (int i = 0; i < voices.size(); ++i) {
    // we reserve space for track 0 to be the bpm track
    create_midi_voice(creator, params, bpm_man, voices.at(i), VoiceId(i),
                      i + 1);
  }
  bpm_man.write_bpm_track(creator, 0);
  return creator.generate_file();
}

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
  return create_midi(params, voices);
}
} // namespace synthlib

// NOLINTBEGIN
#ifdef CFG_TEST
#include "doctest.h"

using namespace synthlib;

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
#endif
// NOLINTEND

#include "synthlib/compiler.hpp"
#include "synthlib/bpm_manager.hpp"
#include "synthlib/command.hpp"
#include "synthlib/event_consumer.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <variant>
#include <vector>

using std::vector, std::string_view, std::variant;

namespace synthlib {

using cursor = string_view::size_type;

static const uint8_t Mb = '+';
static const char *const Mb_s = "+";

std::string CompilerError::gen_msg() const {
  using Kind = CompilerError::Kind;
  std::stringstream msg;
  msg << "Error at line " << _line << ": ";
  switch (_kind) {
  case Kind::InvalidDelay:
    msg << "invalid delay specifier";
    break;
  case Kind::UnclosedDelay:
    msg << "unclosed delay clausule";
    break;
  }
  return msg.str();
}
// Mapping for synthext language
// We replace all occurrences of "Mb", which is the only
// multi-letter token with "+", which is an unused character
// so we can use an array to map characters to commands.
Compiler::Mapping build_map() {
  Compiler::Mapping map{};

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
      throw CompilerError::Kind::InvalidDelay;
      // throw std::invalid_argument("delay specifier is not a number");
    }
  }
  return out;
}

/// Compiles a single line into a voice

Compiler::Voiceline Compiler::compile_line(const std::string &line) const {

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
      throw CompilerError::Kind::UnclosedDelay;
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

/// separates a string into lines without the newline
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

void create_midi_voice(IEventConsumer &consumer, const VoiceManager &params,
                       BpmManager &bpm_man, Compiler::Voiceline &voice,
                       VoiceId voice_id, int track_num) {
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

  consumer.change_track(track_num);

  consumer.change_instrument(channel, instr);

  for (auto command : voice) {
    switch (command.kind()) {
    case CommandKind::Delay:
      consumer.wait_beats(command.amount());
      elapsed += command.amount();
      break;
    case CommandKind::Pause:
      consumer.wait_beats(1);
      elapsed += 1;
      break;
    case CommandKind::PauseOrRepeat:
      if (last.kind() == CommandKind::PlayNote) {
        consumer.play_note(channel, last.note(), octave, volume);
      } else {
        consumer.wait_beats(1);
      }
      elapsed += 1;
      break;
    case CommandKind::PlayNote:
      consumer.play_note(channel, command.note(), octave, volume);
      elapsed += 1;
      break;
    case CommandKind::ChangeInstrument:
      instr = command.instrument();
      consumer.change_instrument(channel, instr);
      break;
    case CommandKind::IncreaseInstrument:
      try {
        instr = Instrument(instr.to_int() + command.amount());
      } catch (...) {
        instr = Instrument(params.get_instrument(voice_id));
      }
      consumer.change_instrument(channel, instr);
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

/// Transforms the list of voice lines into the midi file
void create_midi(IEventConsumer &consumer, const VoiceManager &params,
                 std::vector<Compiler::Voiceline> voices) {
  // If we don't set ticks_per_beat = 8, bpm changes will result
  // in a delay in the first note after the change
  // MidiCreator creator(8);
  BpmManager bpm_man(params.get_bpm());
  // If the file has too many lines, we just get the first 16 voices
  if (voices.size() > (VoiceId::MAX + 1)) {
    voices.resize(VoiceId::MAX + 1);
  }
  for (int i = 0; i < voices.size(); ++i) {
    // we reserve space for track 0 to be the bpm track
    create_midi_voice(consumer, params, bpm_man, voices.at(i), VoiceId(i),
                      i + 1);
  }
  // and write the bpm track in the reserved space
  bpm_man.write_bpm_track(consumer, 0);
  // return creator.generate_file();
}

Compiler::Compiler() { map = build_map(); }

void Compiler::compile(IEventConsumer &consumer,
                       const VoiceManager &voice_params,
                       std::string source) const {
  std::vector<std::string> lines = get_lines(source);
  std::vector<Compiler::Voiceline> voices;
  voices.reserve(lines.size());

  int line_number = 0;
  for (auto &line : lines) {
    ++line_number;
    if (line.empty()) {
      continue;
    }
    try {
      voices.push_back(compile_line(line));
    } catch (CompilerError::Kind err) {
      throw CompilerError(err, line_number);
    }
  }
  create_midi(consumer, voice_params, voices);
}
// void Compiler::compile_to_file(const VoiceManager &voice_params,
//                                std::string source, std::string filename)
//                                const {
//   auto bytes = compile(voice_params, std::move(source));
//   std::ofstream outfile(filename, std::ios::binary);
//   outfile.write((char *)bytes.data(), bytes.size());
//   outfile.close();
// }
} // namespace synthlib

// NOLINTBEGIN
#define CFG_TEST
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
  Compiler cl;
  auto voice = cl.compile_line(s);
  CHECK(voice.size() == 9);
  for (int i = 0; i < 9; ++i) {
    CHECK(voice.at(i) == expected.at(i));
  }
}

TEST_CASE("compiling notes") {
  std::string s = "ABCDEFGHMb";
  Compiler cl;
}
#endif
// NOLINTEND

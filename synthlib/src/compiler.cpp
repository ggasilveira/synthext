#include "synthlib/compiler.hpp"
#include "synthlib/bpm_manager.hpp"
#include "synthlib/command.hpp"
#include "synthlib/event_consumer.hpp"
#include <array>
#include <iostream>
#include <regex>
#include <sstream>
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
void Compiler::build_map() {
  using std::make_shared;

  auto pause_or_repeat = make_shared<PauseOrRepeat>();
  for (int i = 0; i < map.size(); ++i) {
    map.at(i) = pause_or_repeat;
  }

  // note commands
  map['A'] = make_shared<PlayNote>(Note::A);
  map['B'] = make_shared<PlayNote>(Note::B);
  map['C'] = make_shared<PlayNote>(Note::C);
  map['D'] = make_shared<PlayNote>(Note::D);
  map['E'] = make_shared<PlayNote>(Note::E);
  map['F'] = make_shared<PlayNote>(Note::F);
  map['G'] = make_shared<PlayNote>(Note::G);
  map['H'] = make_shared<PlayNote>(Note::Bb);
  map[Mb] = make_shared<PlayNote>(Note::Eb);

  // pause commands
  for (auto c : "abcdefgh") {
    map.at(c) = make_shared<Pause>(1);
  }

  // volume commands
  map[' '] = make_shared<DoubleVolume>();

  // instrument commands
  map['!'] = make_shared<ChangeInstrument>(Midi::Harmonica);
  map[','] = make_shared<ChangeInstrument>(Midi::ChurchOrgan);
  map[';'] = make_shared<ChangeInstrument>(Midi::TubularBells);
  for (auto c : "13579") {
    map.at(c) = make_shared<ChangeInstrument>(Midi::TubularBells);
  }
  for (auto c : "02468") {
    map.at(c) = make_shared<AddInstrument>(c - '0');
  }

  // octave commands
  map['?'] = make_shared<AddOctave>(1);
  map['V'] = make_shared<AddOctave>(-1);

  // bpm commands
  map['>'] = make_shared<AddBpm>(10);
  map['<'] = make_shared<AddBpm>(-10);
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

  Compiler::Voiceline comms;
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
    comms.push_back(std::make_shared<Pause>(delay));
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
  // uint32_t elapsed = 0;
  // last command for pause or repeat
  // Command last;
  Channel channel(voice_id.value());
  VoiceParams initial_params = params.get_voice_params(voice_id);

  consumer.change_track(track_num);
  consumer.change_instrument(channel, initial_params.instrument);

  CommandContext ctx(consumer, bpm_man, channel, initial_params);
  for (const auto &command : voice) {
    command->execute(ctx);
  }

  // for (auto command : voice) {
  //   switch (command.kind()) {
  //   case CommandKind::Delay:
  //     consumer.wait_beats(command.amount());
  //     elapsed += command.amount();
  //     break;
  //   case CommandKind::Pause:
  //     consumer.wait_beats(1);
  //     elapsed += 1;
  //     break;
  //   case CommandKind::PauseOrRepeat:
  //     if (last.kind() == CommandKind::PlayNote) {
  //       consumer.play_note(channel, last.note(), octave, volume);
  //     } else {
  //       consumer.wait_beats(1);
  //     }
  //     elapsed += 1;
  //     break;
  //   case CommandKind::PlayNote:
  //     consumer.play_note(channel, command.note(), octave, volume);
  //     elapsed += 1;
  //     break;
  //   case CommandKind::ChangeInstrument:
  //     instr = command.instrument();
  //     consumer.change_instrument(channel, instr);
  //     break;
  //   case CommandKind::IncreaseInstrument:
  //     try {
  //       instr = Instrument(instr.to_int() + command.amount());
  //     } catch (...) {
  //       instr = Instrument(params.get_instrument(voice_id));
  //     }
  //     consumer.change_instrument(channel, instr);
  //     break;
  //   case CommandKind::IncreaseOctave:
  //     if (octave.value() < Octave::MAX) {
  //       octave = Octave(octave.value() + 1);
  //     } else {
  //       octave = params.get_octave(voice_id);
  //     }
  //     break;
  //   case CommandKind::DecreaseOctave:
  //     if (octave.value() > Octave::MIN) {
  //       octave = Octave(octave.value() - 1);
  //     } else {
  //       octave = params.get_octave(voice_id);
  //     }
  //     break;
  //   case CommandKind::IncreaseBpm:
  //     bpm_man.change_bpm(elapsed, command.amount());
  //     break;
  //   case CommandKind::DecreaseBpm:
  //     bpm_man.change_bpm(elapsed, -command.amount());
  //     break;
  //   case CommandKind::DoubleVolume:
  //     try {
  //       volume = Volume(volume.value() * 2);
  //     } catch (...) {
  //       volume = Volume(Volume::MAX);
  //     }
  //     break;
  //   }
  // last = command;
  //}
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

Compiler::Compiler() { build_map(); }

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

} // namespace synthlib

// NOLINTBEGIN
#ifdef CFG_TEST
#include "doctest.h"
#include "spdlog/spdlog.h"
#include "synthlib/mock_event_consumer.hpp"

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

TEST_CASE("compiling notes") {
  std::string s = "ABCDEFGHMb";
  constexpr int voice_channel = 0;
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  cl.compile(consumer, voices, s);
  const auto &track = consumer.nth_track(1);

  auto check_note = [&](int n, Channel channel, Note note) {
    return track.nth_event(n).as_play_note().note == note;
  };
  CHECK(check_note(1, voice_channel, Note::A));
  CHECK(check_note(2, voice_channel, Note::B));
  CHECK(check_note(3, voice_channel, Note::C));
  CHECK(check_note(4, voice_channel, Note::D));
  CHECK(check_note(5, voice_channel, Note::E));
  CHECK(check_note(6, voice_channel, Note::F));
  CHECK(check_note(7, voice_channel, Note::G));
  CHECK(check_note(8, voice_channel, Note::Bb));
  CHECK(check_note(9, voice_channel, Note::Eb));
}

TEST_CASE("compiling pauses") {
  std::string s = "abcdefgh";
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  cl.compile(consumer, voices, s);
  const auto &track = consumer.nth_track(1);

  for (int i = 0; i < s.size(); ++i) {
    auto ev = track.nth_event(i + 1).as_wait_beats();
    CHECK(ev == mock::WaitBeats(1));
  }
}

TEST_CASE("compiling instrument changes") {
  std::string s = "13579;,!";
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  cl.compile(consumer, voices, s);
  const auto &track = consumer.nth_track(1);

  auto check_instr = [&](int n, Midi midi) {
    return track.nth_event(n).as_change_instrument().instr.midi() == midi;
  };
  Midi midi = voices.get_instrument(0).midi();

  CHECK(check_instr(0, midi));
  CHECK(check_instr(1, Midi::TubularBells));
  CHECK(check_instr(2, Midi::TubularBells));
  CHECK(check_instr(3, Midi::TubularBells));
  CHECK(check_instr(4, Midi::TubularBells));
  CHECK(check_instr(5, Midi::TubularBells));
  CHECK(check_instr(6, Midi::TubularBells));
  CHECK(check_instr(7, Midi::ChurchOrgan));
  CHECK(check_instr(8, Midi::Harmonica));
}

TEST_CASE("compiling increase instrument") {
  std::string s = "02468";
  constexpr int voice_channel = 0;
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  cl.compile(consumer, voices, s);
  const auto &track = consumer.nth_track(1);
  auto check_instr = [&](int n, int midi) {
    return track.nth_event(n).as_change_instrument().instr.to_int() == midi;
  };
  int midi = voices.get_instrument(0).to_int();

  CHECK(check_instr(0, midi));
  CHECK(check_instr(1, midi += 0));
  CHECK(check_instr(2, midi += 2));
  CHECK(check_instr(3, midi += 4));
  CHECK(check_instr(4, midi += 6));
  CHECK(check_instr(5, midi += 8));
}

TEST_CASE("compiling octave changes") {
  std::string s = "?AVA??A?A";
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  voices.override_octave(0, Octave::MAX - 2);
  cl.compile(consumer, voices, s);

  const auto &track = consumer.nth_track(1);
  auto check_oct = [&](int n, Octave expected) {
    return track.nth_event(n).as_play_note().octave == expected;
  };
  CHECK(check_oct(1, Octave::MAX - 1));
  CHECK(check_oct(2, Octave::MAX - 2));
  CHECK(check_oct(3, Octave::MAX));
  CHECK(check_oct(4, Octave::MAX - 2));
}
TEST_CASE("compiling volume changes") {
  std::string s = "A A A         A";
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  voices.override_volume(0, Volume(5));
  cl.compile(consumer, voices, s);

  const auto &track = consumer.nth_track(1);
  auto check_vol = [&](int n, Volume expected) {
    return track.nth_event(n).as_play_note().volume == expected;
  };
  CHECK(check_vol(1, 5));
  CHECK(check_vol(2, 10));
  CHECK(check_vol(3, 20));
  CHECK(check_vol(4, Volume::MAX));
}

TEST_CASE("compiling repeat or pause") {
  spdlog::set_level(spdlog::level::debug);
  std::string s;
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  std::string reppause_chars = "ijklmnopqrstuwxyzIJKLMNOPQRSTUWXYZ";
  for (char reppause : reppause_chars) {
    s.push_back('A');
    s.push_back(reppause);
    s.push_back('a');
    s.push_back(reppause);
  }
  cl.compile(consumer, voices, s);

  const auto &track = consumer.nth_track(1);
  auto check_vol = [&](int n, Volume expected) {
    return track.nth_event(n).as_play_note().volume == expected;
  };
  for (int i = 2; i < s.size(); i += 4) {
    CHECK(track.nth_event(i).as_play_note().note == Note::A);
    CHECK(track.nth_event(i + 2).as_wait_beats().beats == 1);
  }
}

TEST_CASE("compiling bpm change") {
  std::string s = "><a<aa>";
  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  cl.compile(consumer, voices, s);
  const auto &track = consumer.nth_track(0);
  auto check_bpm = [&](int n, mock::SetBpm expected) {
    return track.nth_event(n).as_set_bpm() == expected;
  };
  auto check_wait = [&](int n, mock::WaitBeats expected) {
    return track.nth_event(n).as_wait_beats() == expected;
  };
  Bpm bpm = voices.get_bpm();
  CHECK(check_bpm(0, bpm));

  CHECK(check_wait(1, 0));
  CHECK(check_bpm(2, bpm.add_saturated(10)));

  CHECK(check_wait(3, 0));
  CHECK(check_bpm(4, bpm));

  CHECK(check_wait(5, 1));
  CHECK(check_bpm(6, bpm.sub_saturated(10)));

  CHECK(check_wait(7, 2));
  CHECK(check_bpm(8, bpm));
}

TEST_CASE("compiling multiple voices") {
  std::string s = "ABC\nBCDE\n\nCDEFG";

  Compiler cl;
  mock::MockEventConsumer consumer;
  VoiceManager voices;
  cl.compile(consumer, voices, s);
  const auto &track1 = consumer.nth_track(1);
  const auto &track2 = consumer.nth_track(2);
  const auto &track3 = consumer.nth_track(3);

  /// testing track sizes
  /// size = initial instrument change + notes
  CHECK(track1.size() == 4);
  CHECK(track2.size() == 5);
  CHECK(track3.size() == 6);

  mock::PlayNote tr1fst = track1.nth_event(1).as_play_note();
  mock::PlayNote tr2fst = track2.nth_event(1).as_play_note();
  mock::PlayNote tr3fst = track3.nth_event(1).as_play_note();

  mock::PlayNote tr1lst = track1.nth_event(3).as_play_note();
  mock::PlayNote tr2lst = track2.nth_event(4).as_play_note();
  mock::PlayNote tr3lst = track3.nth_event(5).as_play_note();

  /// testing tracks first notes
  CHECK(tr1fst.note == Note::A);
  CHECK(tr2fst.note == Note::B);
  CHECK(tr3fst.note == Note::C);

  CHECK(tr1fst.octave == voices.get_octave(0));
  CHECK(tr2fst.octave == voices.get_octave(1));
  CHECK(tr3fst.octave == voices.get_octave(2));

  CHECK(tr1fst.volume == voices.get_volume(0));
  CHECK(tr2fst.volume == voices.get_volume(1));
  CHECK(tr3fst.volume == voices.get_volume(2));

  /// testing tracks last notes
  CHECK(tr1lst.note == Note::C);
  CHECK(tr2lst.note == Note::E);
  CHECK(tr3lst.note == Note::G);

  /// testing track initial instruments
  CHECK(track1.nth_event(0).as_change_instrument().instr ==
        voices.get_instrument(0));
  CHECK(track2.nth_event(0).as_change_instrument().instr ==
        voices.get_instrument(1));
  CHECK(track3.nth_event(0).as_change_instrument().instr ==
        voices.get_instrument(2));
}

#endif
// NOLINTEND

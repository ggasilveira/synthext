#include "synthlib/mapper.hpp"
#include "synthlib/command.hpp"
#include <memory>
#include <optional>
#include <string_view>

using namespace synthlib;

using std::optional;

/// uppercase characters A to H generate notes
auto match_note(char ascii) -> optional<Note> {
  auto note = optional<Note>{};
  switch (ascii) {
  case 'A':
    note = Note::A;
    break;
  case 'B':
    note = Note::B;
    break;
  case 'C':
    note = Note::C;
    break;
  case 'D':
    note = Note::D;
    break;
  case 'E':
    note = Note::E;
    break;
  case 'F':
    note = Note::F;
    break;
  case 'G':
    note = Note::G;
    break;
  case 'H':
    note = Note::Bb;
    break;
  default:
    note.reset();
  }
  return note;
}

auto match_any(char ascii, std::string_view chars) {
  for (char c : chars) {
    if (ascii == c) {
      return true;
    }
  }
  return false;
}

template <typename T> std::unique_ptr<ICommand> mkcomm(T command) {
  return std::make_unique<T>(std::move(command));
}

auto repeat(char last_char) -> std::unique_ptr<ICommand> {
  auto maybe_note = match_note(last_char);
  if (maybe_note) {
    return mkcomm(CommandNote(maybe_note.value()));
  }
  return mkcomm(CommandSilence());
}

constexpr auto OTHER_VOWELS = "iouIOU";
constexpr auto OTHER_CONSONANTS = "jklmnpqrstvwxyzJKLMNPQRSTVWXYZ";
constexpr auto EVENS = "02468";
constexpr auto ODDS = "13579";

auto ParseMapper::process(char ascii) -> std::unique_ptr<ICommand> {
  std::unique_ptr<ICommand> comm;
  auto maybe_note = match_note(ascii);
  if (maybe_note) {
    comm = mkcomm(CommandNote(maybe_note.value()));

  } else if (match_any(ascii, "abcdefgh")) {
    comm = mkcomm(CommandSilence());

  } else if (match_any(ascii, " ")) {
    comm = mkcomm(CommandDoubleVolume());

  } else if (match_any(ascii, "!")) {
    comm = mkcomm(CommandChangeMidi(24));

  } else if (match_any(ascii, OTHER_VOWELS)) {
    comm = mkcomm(CommandChangeMidi(110));

  } else if (match_any(ascii, OTHER_CONSONANTS)) {
    comm = repeat(last_char);

  } else if (match_any(ascii, EVENS)) {
    int amount = ascii - '0';
    comm = mkcomm(CommandMoveMidi(amount));

  } else if (match_any(ascii, "?.")) {
    comm = mkcomm(CommandAddOctave(1));

  } else if (match_any(ascii, "\n\r")) {
    comm = mkcomm(CommandChangeMidi(123));

  } else if (match_any(ascii, ";") || match_any(ascii, ODDS)) {
    comm = mkcomm(CommandChangeMidi(15));

  } else if (match_any(ascii, ",")) {
    comm = mkcomm(CommandChangeMidi(114));

  } else {
    comm = repeat(last_char);
  }
  last_char = ascii;
  return comm;
}

auto repeat_creator(const MapperState &stt) -> std::unique_ptr<ICommand> {
  auto maybe_note = match_note(stt.last_char());
  if (maybe_note) {
    return std::make_unique<CommandNote>(maybe_note.value());
  }
  return std::make_unique<CommandSilence>();
}

auto TableMapper::default_table() -> TableMapper {
  using namespace std::literals;
  TableMapper table;

  table.add_simple('A', CommandNote(Note::A));
  table.add_simple('B', CommandNote(Note::B));
  table.add_simple('C', CommandNote(Note::C));
  table.add_simple('D', CommandNote(Note::D));
  table.add_simple('E', CommandNote(Note::E));
  table.add_simple('F', CommandNote(Note::F));
  table.add_simple('G', CommandNote(Note::G));
  table.add_simple('H', CommandNote(Note::Bb));

  table.add_simple("abcdefgh", CommandSilence());

  table.add_simple(' ', CommandDoubleVolume());
  table.add_simple('!', CommandChangeMidi(24));
  table.add_simple("iouIOU", CommandChangeMidi(110));
  table.add_rule("jklmnpqrstvwxyz", repeat_creator);

  table.add_simple('0', CommandMoveMidi(0));
  table.add_simple('2', CommandMoveMidi(2));
  table.add_simple('4', CommandMoveMidi(4));
  table.add_simple('6', CommandMoveMidi(6));
  table.add_simple('8', CommandMoveMidi(8));

  table.add_simple("?.", CommandAddOctave(1));
  table.add_simple("\n\r", CommandChangeMidi(123));
  table.add_simple(";13579", CommandChangeMidi(15));
  table.add_simple(',', CommandChangeMidi(114));

  table.set_default(repeat_creator);
  // table.add_simple("?.",Comma)

  return table;
}

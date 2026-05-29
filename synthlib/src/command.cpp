#include "synthlib/command.hpp"

namespace synthlib {
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

void Pause::execute(CommandContext &ctx) { ctx.consumer().wait_beats(_beats); }
unsigned int Pause::beats_taken() { return _beats; }
void PauseOrRepeat::execute(CommandContext &ctx) {
  if (ctx.last_note) {
    // ctx.consumer().play_note(ctx.last_note);
  }
}
unsigned int PauseOrRepeat::beats_taken() { return 1; }
void PlayNote::execute(CommandContext &ctx) {}
unsigned int PlayNote::beats_taken() { return 1; }
void ChangeInstrument::execute(CommandContext &ctx) {}
void AddOctave::execute(CommandContext &ctx) {}
void AddBpm::execute(CommandContext &ctx) {}
void DoubleVolume::execute(CommandContext &ctx) {}

} // namespace synthlib

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
void PauseOrRepeat::execute(CommandContext &ctx) {
  if (ctx.last_note) {
    //ctx.consumer().play_note(ctx.last_note);
  }
}
void PlayNote::execute(CommandContext &ctx) {}
void ChangeInstrument::execute(CommandContext &ctx) {}
void AddOctave::execute(CommandContext &ctx) {}
void AddBpm::execute(CommandContext &ctx) {}
void DoubleVolume::execute(CommandContext &ctx) {}

} // namespace synthlib

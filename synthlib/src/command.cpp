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

unsigned int Pause::beats_taken() const { return _beats; }
void Pause::_execute(CommandContext &ctx) { ctx.consumer().wait_beats(_beats); }

unsigned int PauseOrRepeat::beats_taken() const { return 1; }
std::optional<Note>
PauseOrRepeat::note_played(const CommandContext &ctx) const {
  return (ctx.last_note());
}
void PauseOrRepeat::_execute(CommandContext &ctx) {
  if (ctx.last_note()) {
    ctx.consumer().play_note(ctx.channel(), ctx.last_note().value(),
                             ctx.octave(), ctx.volume());
  }
}

unsigned int PlayNote::beats_taken() const { return 1; }
std::optional<Note> PlayNote::note_played(const CommandContext &ctx) const {
  return _note;
}
void PlayNote::_execute(CommandContext &ctx) {
  ctx.consumer().play_note(ctx.channel(), _note, ctx.octave(), ctx.volume());
}

void ChangeInstrument::_execute(CommandContext &ctx) {
  ctx.set_instrument(_instr);
}
void AddOctave::_execute(CommandContext &ctx) {
  try {
    Octave octave = ctx.octave().value() + _amount;
    ctx.set_octave(octave);
  } catch (...) {
    ctx.set_octave(ctx.initial_octave());
  }
}
void AddBpm::_execute(CommandContext &ctx) {
  ctx.bpm_manager().change_bpm(ctx.current_beat(), _amount);
}
void DoubleVolume::_execute(CommandContext &ctx) {
  ctx.set_volume(ctx.volume().add_saturated(ctx.volume().value()));
}

void AddInstrument::_execute(CommandContext &ctx) {
  try {
    Instrument instr = ctx.instrument().to_int() + _amount;
    ctx.set_instrument(instr);
  } catch (...) {
    ctx.set_instrument(ctx.initial_instrument());
  }
}

} // namespace synthlib

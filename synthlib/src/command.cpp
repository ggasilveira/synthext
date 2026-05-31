#include "synthlib/command.hpp"
#include "spdlog/spdlog.h"

namespace synthlib {

unsigned int Pause::beats_taken() const { return _beats; }
void Pause::_execute(CommandContext &ctx) { ctx.consumer().wait_beats(_beats); }

unsigned int PauseOrRepeat::beats_taken() const { return 1; }
std::optional<Note>
PauseOrRepeat::note_played(const CommandContext &ctx) const {
  return (ctx.last_note());
}
void PauseOrRepeat::_execute(CommandContext &ctx) {
  if (ctx.last_note()) {
    spdlog::debug("repeating last note");
    ctx.consumer().play_note(ctx.channel(), ctx.last_note().value(),
                             ctx.octave(), ctx.volume());
  } else {
    spdlog::debug("pausing");
    ctx.consumer().wait_beats(1);
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

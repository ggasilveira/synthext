#include "synthlib/command.hpp"
#include "synthlib/player.hpp"
#include <algorithm>

using namespace synthlib;

void CommandNote::execute(IPlayer &player) { player.play_note(note); }
void CommandChangeMidi::execute(IPlayer &player) { player.set_midi(midi); }
void CommandMoveMidi::execute(IPlayer &player) {
  player.set_midi(player.get_midi() + amount);
}

void CommandSilence::execute(IPlayer &player) { player.play_silence(); }

void CommandDoubleVolume::execute(IPlayer &player) {
  int new_volume = player.get_volume() * 2;
  int max_volume = player.max_volume();

  player.set_volume(std::min(new_volume, max_volume));
}
void CommandAddOctave::execute(IPlayer &player) {
  int new_oct = player.get_octave() + amount;
  int max_oct = player.max_octave();

  player.set_octave(std::min(new_oct, max_oct));
}

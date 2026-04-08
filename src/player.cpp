#include "synthlib/player.hpp"
using synthlib::PlayerConfig;
using synthlib::PrintPlayer;

void PrintPlayer::config(const PlayerConfig &config) {
  _bpm = config.bpm();
  _volume = config.volume();
  _max_volume = config.max_volume();
  _octave = config.octave();
  _max_octave = config.max_octave();
  _midi = config.midi();
  _note_beats = config.note_beats();
  _silence_beats = config.silence_beats();
}

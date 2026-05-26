#include "synthlib/player.hpp"
#include <stdexcept>

namespace synthlib {

Player::Player() {
  settings = new_fluid_settings();
  synth = new_fluid_synth(settings);
  player = new_fluid_player(synth);

  // 512 bufsize to avoid audible glitches in linux
  fluid_settings_setint(settings, "audio.period-size", 512);
  audio_driver = new_fluid_audio_driver(settings, synth);
}
Player::~Player() {
  delete_fluid_audio_driver(audio_driver);
  delete_fluid_player(player);
  delete_fluid_synth(synth);
  delete_fluid_settings(settings);
}

void Player::load_soundfont(const std::string &file) {
  if (fluid_synth_sfload(synth, file.c_str(), 1) == FLUID_FAILED) {
    throw std::runtime_error("could not open soundfont file");
  }
}
void Player::load_midi(const std::vector<uint8_t> &midifile) {
  midi_file = midifile;
  fluid_player_stop(player);
  delete_fluid_player(player);
  player = new_fluid_player(synth);
  fluid_player_add_mem(player, midi_file.data(), midi_file.size());
}

void Player::play() {
  if (fluid_player_play(player) == FLUID_FAILED) {
    throw std::runtime_error("failed playing midi file");
  }
}
void Player::stop() {
  if (fluid_player_stop(player) == FLUID_FAILED) {
    throw std::runtime_error("failed pausing midi file");
  }
}
void Player::seek(int ticks) {
  if (fluid_player_seek(player, ticks) == FLUID_FAILED) {
    throw std::runtime_error("failed seeking midi file");
  }
}
void Player::join() { fluid_player_join(player); }

bool Player::is_playing() {
  return fluid_player_get_status(player) == FLUID_PLAYER_PLAYING;
}

int Player::total_ticks() { return fluid_player_get_total_ticks(player); }
int Player::current_tick() { return fluid_player_get_current_tick(player); }

} // namespace synthlib

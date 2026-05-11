#pragma once

#include "synthlib/note.hpp"

namespace synthlib {

class PlayerConfig {
  int _bpm = 0;
  int _volume = 0;
  int _max_volume = 0;
  int _octave = 0;
  int _max_octave = 0;
  int _midi = 0;
  int _note_beats = 0;
  int _silence_beats = 0;

public:
  void set_bpm(int bpm) { _bpm = bpm; }
  void set_volume(int volume) { _volume = volume; }
  void set_max_volume(int volume) { _max_volume = volume; }
  void set_octave(int octave) { _octave = octave; }
  void set_max_octave(int octave) { _max_octave = octave; }
  void set_midi(int instrument) { _midi = instrument; }
  void set_note_beats(int beats) { _note_beats = beats; }
  void set_silence_beats(int beats) { _silence_beats = beats; }

  [[nodiscard]]
  auto bpm() const -> int {
    return _bpm;
  }
  [[nodiscard]]
  auto volume() const -> int {
    return _volume;
  }
  [[nodiscard]]
  auto octave() const -> int {
    return _octave;
  }
  [[nodiscard]]
  auto max_volume() const -> int {
    return _max_volume;
  }
  [[nodiscard]]
  auto max_octave() const -> int {
    return _max_octave;
  }
  [[nodiscard]]
  auto midi() const -> int {
    return _midi;
  }
  [[nodiscard]]
  auto note_beats() const -> int {
    return _note_beats;
  }
  [[nodiscard]]
  auto silence_beats() const -> int {
    return _silence_beats;
  }
};

class IPlayer {
public:
  virtual ~IPlayer() = default;
  // IPlayer(const IPlayer &) = delete;
  // IPlayer(const IPlayer &&) = delete;
  // auto operator=(const IPlayer &) -> IPlayer & = delete;
  // auto operator=(const IPlayer &&) -> IPlayer & = delete;

  // virtual void config(const PlayerConfig &config) = 0;
  virtual auto get_volume() -> int = 0;
  virtual void set_volume(int volume) = 0;
  virtual auto max_volume() -> int = 0;
  virtual void play_note(Note note) = 0;
  virtual void play_silence() = 0;
  virtual auto get_midi() -> int = 0;
  virtual void set_midi(int midi) = 0;
  virtual auto get_octave() -> int = 0;
  virtual void set_octave(int octave) = 0;
  virtual auto max_octave() -> int = 0;
  virtual auto default_octave() -> int = 0;
};

class PrintPlayer : IPlayer {
  int _bpm = 0;
  int _volume = 0;
  int _max_volume = 0;
  int _octave = 0;
  int _max_octave = 0;
  int _midi = 0;
  int _note_beats = 0;
  int _silence_beats = 0;

public:
  void config(const PlayerConfig &config);
  auto get_volume() -> int override;
  void set_volume(int volume) override;
  auto max_volume() -> int override;
  void play_note(Note note) override;
  void play_silence() override;
  auto get_midi() -> int override;
  void set_midi(int midi) override;
  auto get_octave() -> int override;
  void set_octave(int octave) override;
  auto max_octave() -> int override;
  auto default_octave() -> int override;
};

} // namespace synthlib

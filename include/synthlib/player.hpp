#pragma once

namespace synthlib {

class IPlayer {
public:
  virtual ~IPlayer() = default;
  IPlayer(const IPlayer &) = delete;
  IPlayer(const IPlayer &&) = delete;
  auto operator=(const IPlayer &) -> IPlayer & = delete;
  auto operator=(const IPlayer &&) -> IPlayer & = delete;

  virtual auto get_volume() -> int = 0;
  virtual void set_volume(int volume) = 0;
  virtual auto max_volume() -> int = 0;
  virtual void play_note(int note) = 0;
  virtual void play_silence() = 0;
  virtual auto get_midi() -> int = 0;
  virtual void set_midi(int midi) = 0;
  virtual auto get_octave() -> int = 0;
  virtual void set_octave(int octave) = 0;
  virtual auto max_octave() -> int = 0;
  virtual auto default_octave() -> int = 0;
};

class PlayerConfig {
  int bpm = 0;
  int volume = 0;
  int max_volume = 0;
  int octave = 0;
  int max_octave = 0;
  int midi = 0;
  int note_beats = 0;
  int silence_beats = 0;

public:
  void set_bpm(int bpm) { this->bpm = bpm; }
  void set_volume(int vol) { volume = vol; }
  void set_octave(int oct) { octave = oct; }
  void set_max_volume(int vol) { max_volume = vol; }
  void set_midi(int instrument) { this->midi = instrument; }
  void set_note_beats(int beats) { note_beats = beats; }
  void set_silence_beats(int beats) { silence_beats = beats; }

  [[nodiscard]]
  auto get_bpm() const -> int {
    return bpm;
  }
  [[nodiscard]]
  auto get_volume() const -> int {
    return volume;
  }
  [[nodiscard]]
  auto get_octave() const -> int {
    return octave;
  }
  [[nodiscard]]
  auto get_max_volume() const -> int {
    return max_volume;
  }
  [[nodiscard]]
  auto get_midi() const -> int {
    return midi;
  }
  [[nodiscard]]
  auto get_note_beats() const -> int {
    return note_beats;
  }
  [[nodiscard]]
  auto get_silence_beats() const -> int {
    return silence_beats;
  }
};

} // namespace synthlib

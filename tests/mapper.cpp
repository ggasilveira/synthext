#include "synthlib/mapper.hpp"
#include "synthlib/player.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace synthlib;

class MockPlayer : public IPlayer {
public:
  int vol = 0;
  int max_vol = 0;
  int midi = 0;
  int oct = 0;
  int max_oct = 0;
  int default_oct = 0;
  int played_silences = 0;
  std::vector<Note> played_notes;

  int get_volume() override { return vol; }
  void set_volume(int volume) override { vol = volume; }
  int max_volume() override { return max_vol; }
  void play_note(Note note) override { played_notes.push_back(note); }
  void play_silence() override { played_silences++; }
  int get_midi() override { return midi; }
  void set_midi(int _midi) override { midi = _midi; }
  int get_octave() override { return oct; }
  void set_octave(int octave) override { oct = octave; }
  int max_octave() override { return max_oct; }
  int default_octave() override { return default_oct; }
};

TEST_CASE("Command Table follows spec", "[single-file]") {
  MockPlayer mock;
  ParseMapper mapper;
  mapper.process('A')->execute(mock);
  REQUIRE(mock.played_notes.at(0) == Note::A);

}

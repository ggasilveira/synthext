#include "synthlib/midi_creator.hpp"
#include <array>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace synthlib {

const uint8_t NOTE_OFF = 0x8;
const uint8_t NOTE_ON = 0x9;
const uint8_t PROG_CHANGE = 0xC;
const uint8_t CHANNEL_MODE = 0xB;

// NOLINTBEGIN
void varlen(uint32_t number, std::vector<uint8_t> &buf) {
  uint8_t bits7 = number & 0x7F;
  number >>= 7;
  buf.push_back(bits7);
  while (number != 0) {
    bits7 = number & 0x7F | 0x80;
    number >>= 7;
    buf.push_back(bits7);
  }
}

/// Writes a 32bit number in big-endian
void write_32bit(uint32_t number, std::vector<uint8_t> &buf) {
  buf.push_back((number >> 24) & 0xff);
  buf.push_back((number >> 16) & 0xff);
  buf.push_back((number >> 8) & 0xff);
  buf.push_back((number >> 0) & 0xff);
}
// NOLINTEND

uint8_t make_status(uint8_t event, Channel channel) {
  return (event << 4) | (channel.value() & 0xF);
}

/// converts a note + octave into a MIDI note from 0 to 127
uint8_t note2midi(Note note, Octave octave) {
  const int octave_size = 12;
  const int midi_max = 127;

  uint8_t res = static_cast<uint8_t>(note) + (octave.value() + 1) * octave_size;
  if (res > midi_max) {
    throw std::invalid_argument("Note + octave combination too high");
  }
  return res;
}

void midi_event(uint32_t delta_time, uint8_t event, Channel channel,
                uint8_t data1, uint8_t data2, std::vector<uint8_t> &buf) {
  varlen(delta_time, buf);

  // data bytes must have the most significant bit deasserted
  // CHECK((data1 & 0x80) == 0);
  // CHECK((data2 & 0x80) == 0);

  buf.push_back(make_status(event, channel));
  buf.push_back(data1);
  buf.push_back(data2);
  spdlog::debug("MIDI (channel={}, delta={}, event={}"
                "data1={}, data2={})",
                channel.value(), delta_time, event, data1, data2);
}

void midi_event(uint32_t delta_time, uint8_t event, Channel channel,
                uint8_t data1, std::vector<uint8_t> &buf) {

  varlen(delta_time, buf);
  buf.push_back(make_status(event, channel));

  // data bytes must have the most significant bit deasserted
  // CHECK((data1 & 0x80) == 0);

  buf.push_back(data1);
  spdlog::debug("MIDI (channel={}, delta={}, event={}, data1={})",
                channel.value(), delta_time, event, data1);
}

void end_of_track(uint32_t delta_time, std::vector<uint8_t> &buf) {
  const std::array<uint8_t, 3> eot{0xff, 0x2f, 0x00};
  varlen(delta_time, buf);
  for (auto b : eot) {
    buf.push_back(b);
  }
  // spdlog::debug("End Of Track (delta_time={})", delta_time);
}
uint32_t bpm2tempo(Bpm bpm) {
  const uint32_t us_per_min = 60000000;
  return us_per_min / bpm.value();
}

void set_tempo(uint32_t delta_time, Bpm bpm, std::vector<uint8_t> &buf) {
  const std::array<uint8_t, 3> bytes{0xff, 0x51, 0x03};
  varlen(delta_time, buf);
  for (auto b : bytes) {
    buf.push_back(b);
  }
  // get only the first 24 bits
  uint32_t tempo = bpm2tempo(bpm);
  uint8_t b0 = (tempo >> 16) & 0xff;
  uint8_t b1 = (tempo >> 8) & 0xff;
  uint8_t b2 = tempo & 0xff;
  buf.push_back(b0);
  buf.push_back(b1);
  buf.push_back(b2);
  spdlog::debug("Tempo Change (delta={}, bpm={})", delta_time, bpm.value());
}

void note_on(uint32_t delta_time, Channel channel, Note note, Octave octave,
             Volume velocity, std::vector<uint8_t> &buf) {
  midi_event(delta_time, NOTE_ON, channel, note2midi(note, octave),
             velocity.value(), buf);
}
void note_off(uint32_t delta_time, Channel channel, Note note, Octave octave,
              Volume velocity, std::vector<uint8_t> &buf) {
  midi_event(delta_time, NOTE_OFF, channel, note2midi(note, octave),
             velocity.value(), buf);
}

void all_notes_off(uint32_t delta_time, Channel channel,
                   std::vector<uint8_t> &buf) {
  const uint8_t notes_off_c = 127;
  const uint8_t notes_off_v = 0;
  // uint8_t status = make_status(CHANNEL_MODE, channel);
  midi_event(delta_time, CHANNEL_MODE, channel, notes_off_c, notes_off_v, buf);
}

void prog_change(uint32_t delta_time, Channel channel, Instrument instr,
                 std::vector<uint8_t> &buf) {
  midi_event(delta_time, PROG_CHANGE, channel, instr.to_int(), buf);
}

void MidiCreator::goto_track(int track_num) {
  if (track_num >= tracks.size()) {
    tracks.resize(track_num + 1);
  }
  curr_track = track_num;
  spdlog::debug("Track Change (track={})", track_num);
}
void MidiCreator::play_note(Channel channel, Note note, Octave octave,
                            Volume volume) {
  Track &track = tracks.at(curr_track);
  note_on(track.delta, channel, note, octave, volume, track.buf);
  note_off(beat_ticks, channel, note, octave, volume, track.buf);
  track.delta = 0;
}

void MidiCreator::pause_beats(uint32_t n) {
  Track &track = tracks.at(curr_track);
  track.delta += beat_ticks * n;
  spdlog::debug("paused {} beats", n);
}
void MidiCreator::pause_ticks(uint32_t n) {
  Track &track = tracks.at(curr_track);
  track.delta += n;
  spdlog::debug("paused {} ticks", n);
}

void MidiCreator::change_instrument(Channel channel, Instrument instr) {
  Track &track = tracks.at(curr_track);
  prog_change(track.delta, channel, instr, track.buf);
  // we already advanced the delta cursor, so we zero it
  // as we want the next event to play immediately
  track.delta = 0;
}
void MidiCreator::set_bpm(Bpm bpm) {
  Track &track = tracks.at(curr_track);
  set_tempo(track.delta, bpm, track.buf);
  // we already advanced the delta cursor, so we zero it
  // as we want the next event to play immediately
  track.delta = 0;
}

void MidiCreator::write_header(std::vector<uint8_t> &buf) {
  const uint8_t header_len = 6;
  const uint8_t header_format_multitrack = 1;
  // chunk-type
  for (char c : {'M', 'T', 'h', 'd'}) {
    buf.push_back(c);
  }
  // length: 32bit
  buf.push_back(0);
  buf.push_back(0);
  buf.push_back(0);
  buf.push_back(header_len);

  // format: 16bit (1 = )
  buf.push_back(0);
  buf.push_back(header_format_multitrack);

  // ntracks: 16bit
  buf.push_back(0);
  buf.push_back(tracks.size());

  // division: 16bit
  buf.push_back(0);
  buf.push_back(beat_ticks);
}
void MidiCreator::write_track(std::vector<uint8_t> &buf, int track) {
  Track &trk = tracks.at(track);

  // we'll add the End Of Track event temporarily
  // and remove after writing, in case we want to edit
  // the track after generating the file
  auto trk_len_before = trk.buf.size();
  end_of_track(trk.delta, trk.buf);
  auto trk_len = trk.buf.size();

  for (char c : {'M', 'T', 'r', 'k'}) {
    buf.push_back(c);
  }
  write_32bit(trk_len, buf);
  for (uint8_t b : trk.buf) {
    buf.push_back(b);
  }

  trk.buf.resize(trk_len_before);
}

std::vector<uint8_t> MidiCreator::generate_file() {
  std::vector<uint8_t> buf;
  write_header(buf);
  for (int i = 0; i < tracks.size(); ++i) {
    write_track(buf, i);
  }
  return buf;
}

} // namespace synthlib

// NOLINTBEGIN
#ifdef CFG_TEST

#include "doctest.h"

using namespace synthlib;

TEST_CASE("variable length quantity") {
  std::vector<uint8_t> buf;

  varlen(0, buf);
  CHECK(buf.size() == 1);
  CHECK(buf.at(0) == 0);
  buf.clear();

  varlen(0x40, buf);
  CHECK(buf.size() == 1);
  CHECK(buf.at(0) == 0x40);
  buf.clear();

  varlen(0x7F, buf);
  CHECK(buf.size() == 1);
  CHECK(buf.at(0) == 0x7F);
  buf.clear();

  varlen(0x80, buf);
  CHECK(buf.size() == 2);
  CHECK(buf.at(1) == 0x81);
  CHECK(buf.at(0) == 0);
  buf.clear();

  varlen(0x2000, buf);
  CHECK(buf.size() == 2);
  CHECK(buf.at(1) == 0xC0);
  CHECK(buf.at(0) == 0);
  buf.clear();

  varlen(0x3FFF, buf);
  CHECK(buf.size() == 2);
  CHECK(buf.at(1) == 0xFF);
  CHECK(buf.at(0) == 0x7F);
  buf.clear();

  varlen(0x4000, buf);
  CHECK(buf.size() == 3);
  CHECK(buf.at(2) == 0x81);
  CHECK(buf.at(1) == 0x80);
  CHECK(buf.at(0) == 0x00);
  buf.clear();

  varlen(0x100000, buf);
  CHECK(buf.size() == 3);
  CHECK(buf.at(2) == 0xC0);
  CHECK(buf.at(1) == 0x80);
  CHECK(buf.at(0) == 0x00);
  buf.clear();

  varlen(0x1FFFFF, buf);
  CHECK(buf.size() == 3);
  CHECK(buf.at(2) == 0xFF);
  CHECK(buf.at(1) == 0xFF);
  CHECK(buf.at(0) == 0x7F);
  buf.clear();

  varlen(0x200000, buf);
  CHECK(buf.size() == 4);
  CHECK(buf.at(3) == 0x81);
  CHECK(buf.at(2) == 0x80);
  CHECK(buf.at(1) == 0x80);
  CHECK(buf.at(0) == 0x00);
  buf.clear();

  varlen(0x8000000, buf);
  CHECK(buf.size() == 4);
  CHECK(buf.at(3) == 0xC0);
  CHECK(buf.at(2) == 0x80);
  CHECK(buf.at(1) == 0x80);
  CHECK(buf.at(0) == 0x00);
  buf.clear();

  varlen(0xFFFFFFF, buf);
  CHECK(buf.size() == 4);
  CHECK(buf.at(3) == 0xFF);
  CHECK(buf.at(2) == 0xFF);
  CHECK(buf.at(1) == 0xFF);
  CHECK(buf.at(0) == 0x7F);
  buf.clear();
}

TEST_CASE("testing note to MIDI value conversion") {

  CHECK(note2midi(Note::A, Octave(0)) == 21);
  CHECK(note2midi(Note::As, Octave(0)) == 22);
  CHECK(note2midi(Note::B, Octave(0)) == 23);

  CHECK(note2midi(Note::C, Octave(1)) == 24);
  CHECK(note2midi(Note::Cs, Octave(1)) == 25);
  CHECK(note2midi(Note::D, Octave(1)) == 26);
  CHECK(note2midi(Note::Ds, Octave(1)) == 27);
  CHECK(note2midi(Note::E, Octave(1)) == 28);
  CHECK(note2midi(Note::F, Octave(1)) == 29);
  CHECK(note2midi(Note::Fs, Octave(1)) == 30);
  CHECK(note2midi(Note::G, Octave(1)) == 31);
  CHECK(note2midi(Note::Gs, Octave(1)) == 32);
  CHECK(note2midi(Note::A, Octave(1)) == 33);
  CHECK(note2midi(Note::As, Octave(1)) == 34);
  CHECK(note2midi(Note::B, Octave(1)) == 35);

  CHECK(note2midi(Note::C, Octave(2)) == 36);
  CHECK(note2midi(Note::Cs, Octave(2)) == 37);
  CHECK(note2midi(Note::D, Octave(2)) == 38);
  CHECK(note2midi(Note::Ds, Octave(2)) == 39);
  CHECK(note2midi(Note::E, Octave(2)) == 40);
  CHECK(note2midi(Note::F, Octave(2)) == 41);
  CHECK(note2midi(Note::Fs, Octave(2)) == 42);
  CHECK(note2midi(Note::G, Octave(2)) == 43);
  CHECK(note2midi(Note::Gs, Octave(2)) == 44);
  CHECK(note2midi(Note::A, Octave(2)) == 45);
  CHECK(note2midi(Note::As, Octave(2)) == 46);
  CHECK(note2midi(Note::B, Octave(2)) == 47);

  CHECK(note2midi(Note::C, Octave(3)) == 48);
  CHECK(note2midi(Note::Cs, Octave(3)) == 49);
  CHECK(note2midi(Note::D, Octave(3)) == 50);
  CHECK(note2midi(Note::Ds, Octave(3)) == 51);
  CHECK(note2midi(Note::E, Octave(3)) == 52);
  CHECK(note2midi(Note::F, Octave(3)) == 53);
  CHECK(note2midi(Note::Fs, Octave(3)) == 54);
  CHECK(note2midi(Note::G, Octave(3)) == 55);
  CHECK(note2midi(Note::Gs, Octave(3)) == 56);
  CHECK(note2midi(Note::A, Octave(3)) == 57);
  CHECK(note2midi(Note::As, Octave(3)) == 58);
  CHECK(note2midi(Note::B, Octave(3)) == 59);

  CHECK(note2midi(Note::C, Octave(4)) == 60);
  CHECK(note2midi(Note::Cs, Octave(4)) == 61);
  CHECK(note2midi(Note::D, Octave(4)) == 62);
  CHECK(note2midi(Note::Ds, Octave(4)) == 63);
  CHECK(note2midi(Note::E, Octave(4)) == 64);
  CHECK(note2midi(Note::F, Octave(4)) == 65);
  CHECK(note2midi(Note::Fs, Octave(4)) == 66);
  CHECK(note2midi(Note::G, Octave(4)) == 67);
  CHECK(note2midi(Note::Gs, Octave(4)) == 68);
  CHECK(note2midi(Note::A, Octave(4)) == 69);
  CHECK(note2midi(Note::As, Octave(4)) == 70);
  CHECK(note2midi(Note::B, Octave(4)) == 71);

  CHECK(note2midi(Note::C, Octave(5)) == 72);
  CHECK(note2midi(Note::Cs, Octave(5)) == 73);
  CHECK(note2midi(Note::D, Octave(5)) == 74);
  CHECK(note2midi(Note::Ds, Octave(5)) == 75);
  CHECK(note2midi(Note::E, Octave(5)) == 76);
  CHECK(note2midi(Note::F, Octave(5)) == 77);
  CHECK(note2midi(Note::Fs, Octave(5)) == 78);
  CHECK(note2midi(Note::G, Octave(5)) == 79);
  CHECK(note2midi(Note::Gs, Octave(5)) == 80);
  CHECK(note2midi(Note::A, Octave(5)) == 81);
  CHECK(note2midi(Note::As, Octave(5)) == 82);
  CHECK(note2midi(Note::B, Octave(5)) == 83);

  CHECK(note2midi(Note::C, Octave(6)) == 84);
  CHECK(note2midi(Note::Cs, Octave(6)) == 85);
  CHECK(note2midi(Note::D, Octave(6)) == 86);
  CHECK(note2midi(Note::Ds, Octave(6)) == 87);
  CHECK(note2midi(Note::E, Octave(6)) == 88);
  CHECK(note2midi(Note::F, Octave(6)) == 89);
  CHECK(note2midi(Note::Fs, Octave(6)) == 90);
  CHECK(note2midi(Note::G, Octave(6)) == 91);
  CHECK(note2midi(Note::Gs, Octave(6)) == 92);
  CHECK(note2midi(Note::A, Octave(6)) == 93);
  CHECK(note2midi(Note::As, Octave(6)) == 94);
  CHECK(note2midi(Note::B, Octave(6)) == 95);

  CHECK(note2midi(Note::C, Octave(7)) == 96);
  CHECK(note2midi(Note::Cs, Octave(7)) == 97);
  CHECK(note2midi(Note::D, Octave(7)) == 98);
  CHECK(note2midi(Note::Ds, Octave(7)) == 99);
  CHECK(note2midi(Note::E, Octave(7)) == 100);
  CHECK(note2midi(Note::F, Octave(7)) == 101);
  CHECK(note2midi(Note::Fs, Octave(7)) == 102);
  CHECK(note2midi(Note::G, Octave(7)) == 103);
  CHECK(note2midi(Note::Gs, Octave(7)) == 104);
  CHECK(note2midi(Note::A, Octave(7)) == 105);
  CHECK(note2midi(Note::As, Octave(7)) == 106);
  CHECK(note2midi(Note::B, Octave(7)) == 107);

  CHECK(note2midi(Note::C, Octave(8)) == 108);
  CHECK(note2midi(Note::Cs, Octave(8)) == 109);
  CHECK(note2midi(Note::D, Octave(8)) == 110);
  CHECK(note2midi(Note::Ds, Octave(8)) == 111);
  CHECK(note2midi(Note::E, Octave(8)) == 112);
  CHECK(note2midi(Note::F, Octave(8)) == 113);
  CHECK(note2midi(Note::Fs, Octave(8)) == 114);
  CHECK(note2midi(Note::G, Octave(8)) == 115);
  CHECK(note2midi(Note::Gs, Octave(8)) == 116);
  CHECK(note2midi(Note::A, Octave(8)) == 117);
  CHECK(note2midi(Note::As, Octave(8)) == 118);
  CHECK(note2midi(Note::B, Octave(8)) == 119);

  CHECK(note2midi(Note::C, Octave(9)) == 120);
  CHECK(note2midi(Note::Cs, Octave(9)) == 121);
  CHECK(note2midi(Note::D, Octave(9)) == 122);
  CHECK(note2midi(Note::Ds, Octave(9)) == 123);
  CHECK(note2midi(Note::E, Octave(9)) == 124);
  CHECK(note2midi(Note::F, Octave(9)) == 125);
  CHECK(note2midi(Note::Fs, Octave(9)) == 126);
  CHECK(note2midi(Note::G, Octave(9)) == 127);
}

TEST_CASE("writing 32bit in big-endian") {
  uint32_t num = 0x12345678;
  std::vector<uint8_t> buf;
  write_32bit(num, buf);
  CHECK(buf.size() == 4);
  CHECK(buf.at(0) == 0x12);
  CHECK(buf.at(1) == 0x34);
  CHECK(buf.at(2) == 0x56);
  CHECK(buf.at(3) == 0x78);
}

TEST_CASE("note_on MIDI event") {
  std::vector<uint8_t> buf;

  note_on(125, Channel(5), Note::G, Octave(6), Volume(50), buf);
  CHECK(buf.size() == 4);
  CHECK(buf.at(0) == 125);
  CHECK(buf.at(1) == 0x95);
  CHECK(buf.at(2) == note2midi(Note::G, Octave(6)));
  CHECK(buf.at(3) == 50);
}
TEST_CASE("note_off MIDI event") {
  std::vector<uint8_t> buf;

  note_off(100, Channel(4), Note::As, Octave(4), Volume(110), buf);
  CHECK(buf.size() == 4);
  CHECK(buf.at(0) == 100);
  CHECK(buf.at(1) == 0x84);
  CHECK(buf.at(2) == note2midi(Note::As, Octave(4)));
  CHECK(buf.at(3) == 110);
}
#endif
// NOLINTEND

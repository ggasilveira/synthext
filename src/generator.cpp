#include "synthlib/generator.hpp"

#include <vector>
#include <array>
#include <arpa/inet.h>

using std::array;


// const array<uint8_t, 4> midi_header = {'M', 'T', 'h', 'd'};
// const uint16_t midi_format = 0;
// const uint16_t midi_ntracks = 0;

// const uint32_t midi_header_len = sizeof(midi_format) + sizeof(midi_ntracks) + sizeof(midi_division);

// const array<uint8_t, 4> midi_track = {'M', 'T', 'r', 'k'};

// template <size_t N>
// void write_array(std::vector<uint8_t> &buf, array<uint8_t, N> arr) {
//   for (auto byte : arr) {
//     buf.push_back(byte);
//   }
// }

// constexpr uint8_t BYTE1 = 0;
// constexpr uint8_t BYTE2 = 8;
// constexpr uint8_t BYTE3 = 16;
// constexpr uint8_t BYTE4 = 24; 

// // numbers in the MIDI file are big endian
// void write8(std::vector<uint8_t> &buf, uint8_t val) {
//   buf.push_back(val);
// }
// // NOLINTBEGIN
// void write16(std::vector<uint8_t> &buf, uint16_t val) {
//   uint16_t bigendian = htons(val);
//   uint8_t b1 = bigendian >> BYTE1;
//   uint8_t b2 = bigendian >> BYTE2;
//   buf.push_back(b2);
//   buf.push_back(b1);
// }
// void write32(std::vector<uint8_t> &buf, uint32_t val) {
//   uint32_t bigendian = htonl(val);
//   uint8_t b1 = bigendian >> BYTE1;
//   uint8_t b2 = bigendian >> BYTE2;
//   uint8_t b3 = bigendian >> BYTE3;
//   uint8_t b4 = bigendian >> BYTE4;
//   buf.push_back(b4);
//   buf.push_back(b3);
//   buf.push_back(b2);
//   buf.push_back(b1);
// }
// // NOLINTEND

// void write_header(std::vector<uint8_t> &buf, uint16_t ticks_per_quarter_note) {
//   write_array(buf, midi_header);
//   write32(buf, midi_header_len);
//   write16(buf, midi_format);
//   write16(buf, midi_ntracks);

//   // make sure `ticks_per_quarter_note` high bit is 0
//   ticks_per_quarter_note &= 0x7F;
//   write16(buf, ticks_per_quarter_note);
// }

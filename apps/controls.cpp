#include "controls.hpp"
#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include <FL/Fl_Button.H>
#include <FL/fl_callback_macros.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void instrument_fullname(Instrument instr, char *buffer, int buflen) {
  snprintf(buffer, buflen, "%s/%s", instr.group_name(), instr.name());
}

void add_instruments(Fl_Choice &instrs) {
  char buffer[100];
  for (int midi = Instrument::MIDI_MIN; midi <= Instrument::MIDI_MAX; ++midi) {
    Instrument instr(midi);
    instrument_fullname(instr, buffer, 100);
    instrs.add(buffer);
  }
}
void add_voices(Fl_Choice &voices) {
  char buffer[100];
  for (int v_id = VoiceId::MIN; v_id <= VoiceId::MAX; ++v_id) {
    snprintf(buffer, 100, "Voice %d", v_id);

    voices.add(buffer);
  }
}

const synthlib::VoiceManager &Controls::voice_params() { return params; }

void Controls::select_voice() {
  char buffer[100];
  int vid = curr_voice->value();
  volume_slider->value(params.get_volume(vid).value());
  octave_counter->value(params.get_octave(vid).value());
  // instruments->value(params.get_instrument(vid).to_int());
  instrument_fullname(params.get_instrument(vid), buffer, 100);
  instruments->value(instruments->find_index(buffer));
}
void Controls::reset_voice() {
  int vid = curr_voice->value();
  params.reset_voice(vid);
  select_voice();
}

void Controls::build() {
  flex = new Fl_Flex(0, 0, 0, 0, Fl_Flex::COLUMN);
  // control_list = new Fl_Pack(0, 0, 0, 0);

  bpm_slider = new Fl_Value_Slider(0, 0, 0, 0, "Initial BPM");
  bpm_slider->type(FL_HOR_NICE_SLIDER);
  bpm_slider->bounds(Bpm::MIN, Bpm::MAX);
  bpm_slider->precision(0);
  flex->fixed(bpm_slider, 30);

  curr_voice = new Fl_Choice(0, 0, 0, 0, "Voice");
  add_voices(*curr_voice);
  curr_voice->align(FL_ALIGN_BOTTOM);
  FL_METHOD_CALLBACK_0(curr_voice, Controls, this, select_voice);
  flex->fixed(curr_voice, 30);

  volume_slider = new Fl_Value_Slider(0, 0, 0, 0, "Volume");
  volume_slider->type(FL_HOR_NICE_SLIDER);
  volume_slider->bounds(Volume::MIN, Volume::MAX);
  volume_slider->precision(0);
  flex->fixed(volume_slider, 30);

  octave_counter = new Fl_Counter(0, 0, 0, 0, "Octave");
  octave_counter->bounds(Octave::MIN, Octave::MAX);
  octave_counter->precision(0);
  flex->fixed(octave_counter, 30);

  instruments = new Fl_Choice(0, 0, 0, 0, "Instrument");
  add_instruments(*instruments);
  instruments->align(FL_ALIGN_BOTTOM);
  flex->fixed(instruments, 30);

  auto *reset_voice_btn = new Fl_Button(0, 0, 0, 0, "Reset Voice");
  FL_METHOD_CALLBACK_0(reset_voice_btn, Controls, this, reset_voice);
  flex->fixed(reset_voice_btn, 30);

  space = new Fl_Box(0, 0, 0, 0);

  flex->resizable(space);
  flex->gap(30);
  flex->margin(20);

  flex->end();

  curr_voice->value(0);
  select_voice();

  // control_list->end();
}

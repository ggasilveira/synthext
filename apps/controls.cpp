#include "controls.hpp"
#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include <FL/Fl_Button.H>
#include <FL/fl_callback_macros.H>
#include <cstdio>
#include <iostream>

#define BUFFER_SIZE 100
#define WIDGET_HEIGHT_STANDARD 30
#define PLAYBACK_FRAME_HEIGHT 30
#define LAYOUT_GAP 30
#define LAYOUT_MARGIN 20

void instrument_fullname(Instrument instr, char *buffer, int buflen) {
  snprintf(buffer, buflen, "%s/%s", instr.group_name(), instr.name());
}

// NOLINTBEGIN
/// Convert a MIDI value to Fl_Menu_Item userdata
void *midi2userdata(Instrument instr) {
  int midi = instr.to_int();
  // we add 1 to midi to differentiate from the null pointer
  size_t userdata = midi + 1;
  return (void *)userdata;
}
/// COnvert Fl_Menu_Item userdata to midi value
int userdata2midi(void *userdata) {
  size_t userdat = (size_t)userdata;
  return userdat - 1;
}
// NOLINTEND

void add_instruments(Fl_Choice &instrs) {
  char buffer[BUFFER_SIZE];
  for (size_t midi = Instrument::MIDI_MIN; midi <= Instrument::MIDI_MAX;
       ++midi) {
    Instrument instr(midi);
    instrument_fullname(instr, buffer, BUFFER_SIZE);
    instrs.add(buffer, 0, nullptr, midi2userdata(instr));
  }
}
void add_voices(Fl_Choice &voices) {
  char buffer[BUFFER_SIZE];
  for (int v_id = VoiceId::MIN; v_id <= VoiceId::MAX; ++v_id) {
    snprintf(buffer, BUFFER_SIZE, "Voice %d", v_id);
    voices.add(buffer);
  }
}

const synthlib::VoiceManager &Controls::voice_params() { return params; }

void Controls::select_voice() {
  int vid = curr_voice->value();
  volume_slider->value(params.get_volume(vid).value());
  octave_counter->value(params.get_octave(vid).value());
  Instrument instr = params.get_instrument(vid);
  const auto *item =
      instruments->find_item_with_user_data(midi2userdata(instr));
  instruments->value(item);
}
void Controls::reset_voice() {
  int vid = curr_voice->value();
  params.reset_voice(vid);
  select_voice();
}
void Controls::set_instrument() {
  int vid = curr_voice->value();
  int midi = userdata2midi(instruments->mvalue()->user_data());
  params.override_instrument(vid, Instrument(midi));
}
void Controls::set_octave() {
  int vid = curr_voice->value();

  Octave oct((int)octave_counter->value());
  params.override_octave(vid, oct);
}
void Controls::set_volume() {
  int vid = curr_voice->value();
  params.override_volume(vid, Volume((int)volume_slider->value()));
}
void Controls::set_bpm() { params.set_bpm(Bpm((int)bpm_slider->value())); }

void Controls::call_play() {
  if (play_cb) {
    play_cb();
  }
}
void Controls::call_stop() {
  if (stop_cb) {
    stop_cb();
  }
}

void Controls::build() {
  flex = new Fl_Flex(0, 0, 0, 0, Fl_Flex::COLUMN);
  // control_list = new Fl_Pack(0, 0, 0, 0);

  bpm_slider = new Fl_Value_Slider(0, 0, 0, 0, "Initial BPM");
  bpm_slider->type(FL_HOR_NICE_SLIDER);
  bpm_slider->bounds(Bpm::MIN, Bpm::MAX);
  bpm_slider->precision(0);
  bpm_slider->value(Bpm::DEFAULT);
  FL_METHOD_CALLBACK_0(bpm_slider, Controls, this, set_bpm);
  flex->fixed(bpm_slider, WIDGET_HEIGHT_STANDARD);

  curr_voice = new Fl_Choice(0, 0, 0, 0, "Voice");
  add_voices(*curr_voice);
  curr_voice->align(FL_ALIGN_BOTTOM);
  FL_METHOD_CALLBACK_0(curr_voice, Controls, this, select_voice);
  flex->fixed(curr_voice, WIDGET_HEIGHT_STANDARD);

  volume_slider = new Fl_Value_Slider(0, 0, 0, 0, "Volume");
  volume_slider->type(FL_HOR_NICE_SLIDER);
  volume_slider->bounds(Volume::MIN, Volume::MAX);
  volume_slider->precision(0);
  FL_METHOD_CALLBACK_0(volume_slider, Controls, this, set_volume);
  flex->fixed(volume_slider, WIDGET_HEIGHT_STANDARD);

  octave_counter = new Fl_Counter(0, 0, 0, 0, "Octave");
  octave_counter->bounds(Octave::MIN, Octave::MAX);
  octave_counter->precision(0);
  FL_METHOD_CALLBACK_0(octave_counter, Controls, this, set_octave);
  flex->fixed(octave_counter, WIDGET_HEIGHT_STANDARD);

  instruments = new Fl_Choice(0, 0, 0, 0, "Instrument");
  add_instruments(*instruments);
  instruments->align(FL_ALIGN_BOTTOM);
  FL_METHOD_CALLBACK_0(instruments, Controls, this, set_instrument);
  flex->fixed(instruments, WIDGET_HEIGHT_STANDARD);

  auto *reset_voice_btn = new Fl_Button(0, 0, 0, 0, "Reset Voice");
  FL_METHOD_CALLBACK_0(reset_voice_btn, Controls, this, reset_voice);
  flex->fixed(reset_voice_btn, WIDGET_HEIGHT_STANDARD);

  auto *playback_frame = new Fl_Flex(0, 0, 0, 0, Fl_Flex::ROW);
  play_button = new Fl_Button(0, 0, 0, 0, "Play");
  stop_button = new Fl_Button(0, 0, 0, 0, "Stop");
  FL_METHOD_CALLBACK_0(play_button, Controls, this, call_play);
  FL_METHOD_CALLBACK_0(stop_button, Controls, this, call_stop);

  playback_frame->end();
  flex->fixed(playback_frame, PLAYBACK_FRAME_HEIGHT);

  space = new Fl_Box(0, 0, 0, 0);

  flex->resizable(space);
  flex->gap(LAYOUT_GAP);
  flex->margin(LAYOUT_MARGIN);

  flex->end();

  curr_voice->value(0);
  select_voice();

  // control_list->end();
}

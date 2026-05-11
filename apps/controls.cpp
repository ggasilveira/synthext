#include "controls.hpp"
#include <string.h>
#include <stdlib.h>
#include "synthlib/midi.hpp"
#include <stdio.h>

void add_instruments(Fl_Choice &instrs) {
  char buffer[100];
  for (int midi = Instrument::MIDI_MIN; midi <= Instrument::MIDI_MAX; ++midi) {
    Instrument instr(midi);
    snprintf(buffer, 100, "%s/%s", instr.group_name(), instr.name());

    instrs.add(buffer);
  }
}


void Controls::build() {
  flex = new Fl_Flex(0, 0, 0, 0, Fl_Flex::COLUMN);
  // control_list = new Fl_Pack(0, 0, 0, 0);

  bpm_slider = new Fl_Value_Slider(0, 0, 0, 0, "BPM");
  bpm_slider->type(FL_HOR_NICE_SLIDER);
  bpm_slider->bounds(1, BPM_MAX);
  bpm_slider->precision(0);
  flex->fixed(bpm_slider, 30);

  volume_slider = new Fl_Value_Slider(0, 0, 0, 0, "Volume");
  volume_slider->type(FL_HOR_NICE_SLIDER);
  volume_slider->bounds(0, VOL_MAX);
  volume_slider->precision(0);
  flex->fixed(volume_slider, 30);

  instruments = new Fl_Choice(0, 0, 0, 0, "Instrument");
  add_instruments(*instruments);
  instruments->align(FL_ALIGN_BOTTOM);
  flex->fixed(instruments, 30);

  space = new Fl_Box(0, 0, 0, 0);

  flex->resizable(space);
  flex->gap(30);
  flex->margin(20);

  flex->end();

  // control_list->end();
  
}

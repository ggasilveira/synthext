#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Choice.H>
#include <synthlib/midi.hpp>
#include <vector>

struct Voice {
  int octave = 0;
  int volume = 0;
  Instrument instrument;
  
};

class VoiceManager {
  Fl_Value_Slider *volume_slider = nullptr;
  Fl_Counter* octave_counter = nullptr;
  Fl_Choice *instrument_choice = nullptr;
  std::vector<Voice> voices;
};


class Controls {
  static constexpr int BPM_MAX = 120;
  static constexpr int VOL_MAX = 100;
public:
  Controls() {}
  void build();
  Fl_Widget *root() {
    return flex;
  }
private:
  
  Fl_Flex *flex = nullptr;
  Fl_Value_Slider *bpm_slider = nullptr;
  Fl_Value_Slider* volume_slider = nullptr;
  Fl_Counter* octave_counter = nullptr;
  Fl_Choice *instruments = nullptr;
  
  Fl_Box* space = nullptr;


};

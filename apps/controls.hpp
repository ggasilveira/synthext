#pragma once

#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
#include "synthlib/voice_manager.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Value_Slider.H>
#include <functional>
#include <map>
#include <vector>

using namespace synthlib;

/// class for controlling voice and song parameters
class Controls {

public:
  Controls() {}
  void build();
  void on_play(std::function<void()> callback) {
    play_cb = std::move(callback);
  }
  void on_stop(std::function<void()> callback) {
    stop_cb = std::move(callback);
  }

  const synthlib::VoiceManager &voice_params();
  Fl_Widget *root() { return flex; }

private:
  void select_voice();
  void set_instrument();
  void set_octave();
  void set_volume();
  void reset_voice();
  void set_bpm();

  void call_play();
  void call_stop();
  Fl_Flex *flex = nullptr;
  Fl_Value_Slider *bpm_slider = nullptr;
  Fl_Value_Slider *volume_slider = nullptr;
  Fl_Counter *octave_counter = nullptr;
  Fl_Choice *instruments = nullptr;
  Fl_Choice *curr_voice = nullptr;
  Fl_Button *play_button = nullptr;
  Fl_Button *stop_button = nullptr;
  synthlib::VoiceManager params;

  Fl_Box *space = nullptr;

  std::function<void()> play_cb;
  std::function<void()> stop_cb;
};

#pragma once

#include "controls.hpp"
#include "synthlib/compiler.hpp"
#include "synthlib/player.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Value_Slider.H>

#define CALLBACK(classname, method)                                            \
  void method(Fl_Widget *widget);                                              \
  static void method##_cb(Fl_Widget *widget, void *stt) {                      \
    static_cast<classname *>(stt)->method(widget);                             \
  }

class SynthApp {
  static constexpr int MENU_BAR_HEIGHT = 25;
  static constexpr int EDITOR_FONT_SIZE = 18;
  static constexpr int EDITOR_LINE_NUMBER_SIZE = 12;
  static constexpr int CONTROLS_WIDTH = 300;
  static constexpr int INFO_WIDTH = 600;
  static constexpr int INFO_HEIGHT = 400;

public:
  SynthApp() : controls(new Controls) {}
  void build(Fl_Window *window);

private:
  Fl_Menu_Bar *menu_bar = nullptr;

  Fl_Flex *frame = nullptr;
  Fl_Text_Buffer *text_buffer = nullptr;
  Fl_Text_Editor *text_editor = nullptr;

  Controls *controls = nullptr;
  Fl_Window *main_window = nullptr;

  Fl_Window *info_window = nullptr;
  Fl_Text_Buffer *info_text_buffer = nullptr;
  Fl_Text_Display *info_text_display = nullptr;

  synthlib::Player player;
  synthlib::Compiler compiler;

  CALLBACK(SynthApp, on_save_text);
  CALLBACK(SynthApp, on_load_text);
  CALLBACK(SynthApp, on_save_midi);
  CALLBACK(SynthApp, on_show_info);
  CALLBACK(SynthApp, on_main_window_close);

  void build_menu_bar();
  void build_main_frame();
  void build_text_editor();
  void build_controls();
  void build_playback();
  void build_info_window();
  void play_midi();
  void stop_midi();
  void playpause_midi();
  std::vector<uint8_t> compile_midi();
};

#include "ui.hpp"
#include "synthlib/compiler.hpp"
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/fl_callback_macros.H>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string_view>

void SynthApp::on_save_text(Fl_Widget *widget) {
  Fl_Native_File_Chooser file_chooser;
  file_chooser.title("Save File As...");
  file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  if (file_chooser.show() == 0) {
    text_buffer->savefile(file_chooser.filename());
  }
}
void SynthApp::on_load_text(Fl_Widget *widget) {
  Fl_Native_File_Chooser file_chooser;
  file_chooser.title("Load File");
  file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
  if (file_chooser.show() == 0) {
    text_buffer->loadfile(file_chooser.filename());
  }
}
std::vector<uint8_t> SynthApp::compile_midi() {
  return synthlib::compile(controls->voice_params(), text_buffer->text());
}
void SynthApp::on_save_midi(Fl_Widget *widget) {
  auto midifile = compile_midi();
  Fl_Native_File_Chooser file_chooser;
  file_chooser.title("Save File As...");
  file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  file_chooser.filter("MIDI Files (*.mid)");
  if (file_chooser.show() == 0) {
    std::ofstream outfile(file_chooser.filename(), std::ios::binary);
    outfile.write((char *)midifile.data(), midifile.size());
    outfile.close();
  }
}

void SynthApp::build(Fl_Window &window) {

  window.begin();

  // Putting the menu bar in the flex wasn't working for
  // some reason, so we're putting outside and calculating
  // the sizes manually.
  build_menu_bar(window);

  // the dimensions of the frame which will hold all
  // the other widgets.
  int frame_x = 0;
  int frame_y = menu_bar->h();
  int frame_w = window.w();
  int frame_h = window.h() - menu_bar->h();

  frame = new Fl_Flex(frame_x, frame_y, frame_w, frame_h, Fl_Flex::ROW);

  build_text_editor();
  controls->build();
  build_playback();

  frame->fixed(controls->root(), CONTROLS_WIDTH);
  frame->fixed(playstop_button, 50);
  frame->resizable(text_editor);

  frame->end();

  window.resizable(frame);
  window.end();
}

void SynthApp::build_menu_bar(Fl_Window &window) {
  menu_bar = new Fl_Menu_Bar(0, 0, window.w(), MENU_BAR_HEIGHT);
  menu_bar->add("File/Load Text", 0, SynthApp::on_load_text_cb, this);
  menu_bar->add("File/Save Text", 0, SynthApp::on_save_text_cb, this);
  menu_bar->add("File/Save MIDI", 0, SynthApp::on_save_midi_cb, this);
}

void text_changed_cb(int pos, int n_ins, int n_del, int n_restyled,
                     const char *deleted_text, void *changed_var) {
  *static_cast<bool *>(changed_var) = true;
}

void SynthApp::build_text_editor() {
  text_buffer = new Fl_Text_Buffer();
  text_editor = new Fl_Text_Editor(0, 0, 0, 0);
  text_editor->buffer(text_buffer);
  text_editor->textfont(FL_COURIER);
  text_editor->textsize(EDITOR_FONT_SIZE);
  text_editor->cursor_style(Fl_Text_Display::SIMPLE_CURSOR);

  // set line numbering
  text_editor->linenumber_width(EDITOR_LINE_NUMBER_SIZE * 2);
  text_editor->linenumber_format("%d");

  // text_buffer->add_modify_callback(text_changed_cb, &text_changed);
}

void SynthApp::playpause_midi() {
  player.load_midi(compile_midi());
  player.seek(0);
  player.play();
}
void SynthApp::build_playback() {
  player.load_soundfont("assets/general.sf2");

  playstop_button = new Fl_Button(0, 0, 0, 0, "Play/Stop");
  FL_METHOD_CALLBACK_0(playstop_button, SynthApp, this, playpause_midi);
}

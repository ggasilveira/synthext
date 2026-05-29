#include "ui.hpp"
#include "synthlib/compiler.hpp"
#include "synthlib/midi_file_writer.hpp"
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/fl_ask.H>
#include <FL/fl_callback_macros.H>
#include <fstream>
#include <functional>
#include <iostream>

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
  MidifileConsumer consumer;
  compiler.compile(consumer, controls->voice_params(), text_buffer->text());
  return consumer.generate_file();
}

void save_binary_file(std::vector<uint8_t> bytes, std::string filename) {
  std::ofstream outfile(filename, std::ios::binary);
  outfile.write((char *)bytes.data(), bytes.size());
  outfile.close();
}

void show_compile_error(const CompilerError &err) {
  fl_message_title("Error compiling file");
  fl_message("%s", err.what());
}

void SynthApp::on_save_midi(Fl_Widget *widget) {
  try {
    auto midifile = compile_midi();
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Save File As...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    file_chooser.filter("MIDI Files (*.mid)");
    if (file_chooser.show() == 0) {
      save_binary_file(midifile, file_chooser.filename());
    }
  } catch (CompilerError err) {
    show_compile_error(err);
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
  controls->on_play([&]() { play_midi(); });
  controls->on_stop([&]() { stop_midi(); });
  build_playback();

  frame->fixed(controls->root(), CONTROLS_WIDTH);
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

void SynthApp::play_midi() {
  try {
    player.load_midi(compile_midi());
    player.seek(0);
    player.play();

  } catch (CompilerError err) {
    show_compile_error(err);
  }
}
void SynthApp::stop_midi() { player.stop(); }
void SynthApp::build_playback() { player.load_soundfont("assets/general.sf2"); }

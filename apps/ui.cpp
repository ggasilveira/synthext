#include "ui.hpp"
#include "synthlib/compiler.hpp"
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/fl_ask.H>
#include <FL/fl_callback_macros.H>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <string_view>
#include <iterator>

// forward declaration for helper defined later in this file
static bool choose_file(Fl_Native_File_Chooser::Type type,
                        std::string &out_filename,
                        const char *title,
                        const char *filter = nullptr);

void SynthApp::on_save_text(Fl_Widget *widget) {
  std::string filename;
  if (choose_file(Fl_Native_File_Chooser::BROWSE_SAVE_FILE,
                  filename,
                  "Save File As...",
                  "Text Files (*.txt)")) {
    text_buffer->savefile(filename.c_str());
  }
}

void SynthApp::on_load_text(Fl_Widget *widget) {
  std::string filename;
  if (choose_file(Fl_Native_File_Chooser::BROWSE_FILE,
                  filename,
                  "Load File",
                  "Text Files (*.txt)")) {
    text_buffer->loadfile(filename.c_str());
  }
}
std::vector<uint8_t> SynthApp::compile_midi() {

  return compiler.compile(controls->voice_params(), text_buffer->text());
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
  std::string filename;
  if (choose_file(Fl_Native_File_Chooser::BROWSE_SAVE_FILE,
                  filename,
                  "Save File As...",
                  "MIDI Files (*.mid)")) {
    save_binary_file(compile_midi(), filename);
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

  frame->fixed(controls->root(), CONTROLS_WIDTH);
  frame->resizable(text_editor);

  frame->end();

  window.resizable(frame);
  window.end();

  build_playback();
}

void SynthApp::build_menu_bar(Fl_Window &window) {
  menu_bar = new Fl_Menu_Bar(0, 0, window.w(), MENU_BAR_HEIGHT);
  menu_bar->add("File/Load Text", 0, SynthApp::on_load_text_cb, this);
  menu_bar->add("File/Save Text", 0, SynthApp::on_save_text_cb, this);
  menu_bar->add("File/Save MIDI", 0, SynthApp::on_save_midi_cb, this);
  menu_bar->add("info/How to Use", 0, SynthApp::on_show_info_cb, this);
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

void SynthApp::on_show_info(Fl_Widget *widget) {
  // build absolute path to info file relative to this source
  std::string info_path = std::string(__FILE__);
  info_path = info_path.substr(0, info_path.rfind("apps"));
  info_path += "assets/how_to_use.txt";

  std::ifstream infile(info_path);
  if (!infile) {
    fl_message_title("info file not found");
    fl_message("Could not open info file: %s", info_path.c_str());
    return;
  }
  std::string content((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

  auto *info_win = new Fl_Double_Window(600, 400, "Synthext — How to Use");
  info_win->begin();
  auto *buf = new Fl_Text_Buffer();
  auto *disp = new Fl_Text_Display(10, 10, 580, 380);
  disp->buffer(buf);
  buf->text(content.c_str());
  info_win->end();
  info_win->show();
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
void SynthApp::build_playback() { 
  // Use absolute path for soundfont
  std::string soundfont_path = std::string(__FILE__);
  soundfont_path = soundfont_path.substr(0, soundfont_path.rfind("apps"));
  soundfont_path += "assets/general.sf2";
  player.load_soundfont(soundfont_path.c_str()); 
}

static bool choose_file(Fl_Native_File_Chooser::Type type,
                        std::string &out_filename,
                        const char *title,
                        const char *filter) {
  Fl_Native_File_Chooser chooser;
  chooser.title(title);
  chooser.type(type);
  if (filter) {
    chooser.filter(filter);
  }
  if (chooser.show() == 0) {
    out_filename = chooser.filename();
    return true;
  }
  return false;
}


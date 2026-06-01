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
#include <iterator>

#define WIDTH 600
#define HEIGHT 400
#define HORIZONTAL_POSITION 10
#define VERTICAL_POSITION 10
#define TEXT_WIDTH 580
#define TEXT_HEIGHT 380

#include "usage_guide.hpp"

// forward declarations for helpers defined later in this file
static bool choose_file(Fl_Native_File_Chooser::Type type,
                        std::string &out_filename, const char *title,
                        const char *filter = nullptr);

static std::string asset_path(const char *relative_path);

void SynthApp::on_save_text(Fl_Widget *widget) {
  std::string filename;
  if (choose_file(Fl_Native_File_Chooser::BROWSE_SAVE_FILE, filename,
                  "Save File As...", "Text Files (*.txt)")) {
    text_buffer->savefile(filename.c_str());
  }
}

void SynthApp::on_load_text(Fl_Widget *widget) {
  std::string filename;
  if (choose_file(Fl_Native_File_Chooser::BROWSE_FILE, filename, "Load File",
                  "Text Files (*.txt)")) {
    text_buffer->loadfile(filename.c_str());
  }
}
std::vector<uint8_t> SynthApp::compile_midi() {
  // we need ticks per beat = 8 to avoid notes delaying after bpm change
  constexpr int ticks_per_beat = 8;
  MidifileConsumer consumer(ticks_per_beat);
  compiler.compile(consumer, controls->voice_params(), text_buffer->text());
  return consumer.generate_file();
}

void show_compile_error(const CompilerError &err) {
  fl_message_title("Error compiling file");
  fl_message("%s", err.what());
}

void save_binary_file(const std::vector<uint8_t> &bytes,
                      const std::string &filename) {
  std::ofstream outfile(filename, std::ios::binary);
  outfile.write((char *)bytes.data(), bytes.size());
  outfile.close();
}

void SynthApp::on_save_midi(Fl_Widget *widget) {
  try {
    auto midi_bytes = compile_midi();
    std::string filename;
    if (choose_file(Fl_Native_File_Chooser::BROWSE_SAVE_FILE, filename,
                    "Save File As...", "MIDI Files (*.mid)")) {
      save_binary_file(midi_bytes, filename);
    }
  } catch (const CompilerError &err) {
    show_compile_error(err);
  }
}
void SynthApp::on_main_window_close(Fl_Widget *widget) {
  if (info_window) {
    info_window->hide();
  }
  main_window->hide();
}
void SynthApp::build(Fl_Window *window) {
  main_window = window;
  main_window->begin();

  build_menu_bar();
  build_main_frame();

  main_window->resizable(frame);
  main_window->end();
  main_window->callback(on_main_window_close_cb, this);

  build_playback();
}

void SynthApp::build_menu_bar() {
  menu_bar = new Fl_Menu_Bar(0, 0, main_window->w(), MENU_BAR_HEIGHT);
  menu_bar->add("File/Load Text", 0, SynthApp::on_load_text_cb, this);
  menu_bar->add("File/Save Text", 0, SynthApp::on_save_text_cb, this);
  menu_bar->add("File/Save MIDI", 0, SynthApp::on_save_midi_cb, this);
  menu_bar->add("Help/Usage Guide", 0, SynthApp::on_show_info_cb, this);
}

void SynthApp::build_main_frame() {
  auto &win = *main_window;
  int frame_y = menu_bar->h();
  frame = new Fl_Flex(0, frame_y, win.w(), win.h() - frame_y, Fl_Flex::ROW);

  build_text_editor();
  build_controls();

  frame->fixed(controls->root(), CONTROLS_WIDTH);
  frame->resizable(text_editor);

  frame->end();
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
}

void SynthApp::on_show_info(Fl_Widget *widget) {
  if (!info_window) {
    build_info_window();
  }
  info_window->show();
}

void SynthApp::play_midi() {
  try {
    player.load_midi(compile_midi());
    player.seek(0);
    player.play();
  } catch (const CompilerError &err) {
    show_compile_error(err);
  }
}
void SynthApp::stop_midi() { player.stop(); }
void SynthApp::build_controls() {
  controls->build();
  controls->on_play([this]() { play_midi(); });
  controls->on_stop([this]() { stop_midi(); });
}

void SynthApp::build_playback() {
  std::string soundfont_path = asset_path("assets/general.sf2");
  player.load_soundfont(soundfont_path.c_str());
}

void SynthApp::build_info_window() {
  info_window = new Fl_Double_Window(WIDTH, HEIGHT, "Synthext — How to Use");
  info_window->begin();
  info_text_buffer = new Fl_Text_Buffer();
  info_text_display = new Fl_Text_Display(
      HORIZONTAL_POSITION, VERTICAL_POSITION, TEXT_WIDTH, TEXT_HEIGHT);
  info_text_display->buffer(info_text_buffer);
  info_text_buffer->text(usage_guide_txt);
  info_window->end();
  info_window->resizable(info_text_display);
}
static bool choose_file(Fl_Native_File_Chooser::Type type,
                        std::string &out_filename, const char *title,
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

static std::string asset_path(const char *relative_path) {
  std::string asset_dir = std::string(__FILE__);
  asset_dir = asset_dir.substr(0, asset_dir.rfind("apps"));
  asset_dir += relative_path;
  return asset_dir;
}

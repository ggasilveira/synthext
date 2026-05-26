#include <FL/Fl_Window.H>
#include <FL/platform_types.h>
#include <iostream>
#include <memory>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <spdlog/cfg/env.h>

#include <synthlib/midi_creator.hpp>

#include "ui.hpp"

using namespace synthlib;
// int main() {
//   spdlog::cfg::load_env_levels();
//   MidiCreator creator(1);
//   creator.goto_track(1);
//   creator.change_instrument(0, Midi::AcousticGrandPiano);

//   creator.play_note(0, Note::A, 6, 100);
//   creator.play_note(0, Note::A, 6, 100);
//   creator.play_note(0, Note::A, 6, 100);
//   creator.play_note(0, Note::A, 6, 100);
//   creator.play_note(0, Note::A, 6, 100);
//   creator.play_note(0, Note::A, 6, 100);
//   creator.play_note(0, Note::A, 6, 100);
//   creator.play_note(0, Note::A, 6, 100);

//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);
//   creator.play_note(0, Note::B, 6, 100);

//   creator.goto_track(0);
//   creator.pause_beats(8);
//   creator.set_bpm(400);

//   Player player;
//   player.load_soundfont("assets/general.sf2");
//   player.load_midi(creator.generate_file());
//   player.play();
//   player.join();
// }

auto main() -> int {
  spdlog::cfg::load_env_levels();
  auto *app_window = new Fl_Double_Window(800, 600, "Synthext");

  auto *app = new SynthApp();
  app->build(*app_window);

  app_window->show();
  return Fl::run();
}

#include <FL/Fl_Window.H>
#include <FL/platform_types.h>
#include <iostream>
#include <memory>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>

#include "ui.hpp"

auto main() -> int {
  auto *app_window = new Fl_Double_Window(800, 600, "Synthext");

  auto *app = new SynthApp();
  app->build(*app_window);

  app_window->show();
  return Fl::run();
}

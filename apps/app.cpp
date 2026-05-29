
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <spdlog/cfg/env.h>

#include "ui.hpp"

using namespace synthlib;

auto main() -> int {
  spdlog::cfg::load_env_levels();
  auto *app_window = new Fl_Double_Window(800, 600, "Synthext");

  auto *app = new SynthApp();
  app->build(*app_window);

  app_window->show();
  return Fl::run();
}

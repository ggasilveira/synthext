
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <spdlog/cfg/env.h>

#include "ui.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace synthlib;

auto main() -> int {
  spdlog::cfg::load_env_levels();
  auto *app_window = new Fl_Double_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Synthext");

  auto *app = new SynthApp();
  app->build(*app_window);

  app_window->show();
  return Fl::run();
}

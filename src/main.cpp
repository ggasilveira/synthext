#include "synthlib/compiler.hpp"
#include "synthlib/voice.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::ifstream input("test.synth");
  std::string content((std::istreambuf_iterator<char>(input)),
                      std::istreambuf_iterator<char>());
  synthlib::VoiceManager params;
  synthlib::compile(params, content);
}

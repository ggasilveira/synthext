#pragma once
#include "synthlib/voice.hpp"
#include <string>
#include <vector>

namespace synthlib {

std::vector<uint8_t> compile(const VoiceManager &params, std::string source);

} // namespace synthlib

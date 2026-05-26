find_package(FluidSynth CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(SOURCE_FILES
  src/instrument.cpp
  src/voice_manager.cpp
  src/midi_creator.cpp
  src/compiler.cpp
  src/bpm_manager.cpp
  src/command.cpp
  src/player.cpp
)
set(INCLUDE_DIRS
  include
  lib/include
)
add_library(synthlib STATIC ${SOURCE_FILES})

target_include_directories(synthlib PUBLIC ${INCLUDE_DIRS})
target_compile_features(synthlib PUBLIC cxx_std_17)
target_link_libraries(synthlib PRIVATE FluidSynth::libfluidsynth)
target_link_libraries(synthlib PRIVATE spdlog::spdlog)

add_executable(test_synthlib src/test_main.cpp ${SOURCE_FILES})
target_include_directories(test_synthlib PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(test_synthlib PRIVATE CFG_TEST)
target_compile_features(test_synthlib PRIVATE cxx_std_17)
target_link_libraries(test_synthlib PRIVATE FluidSynth::libfluidsynth)
target_link_libraries(test_synthlib PRIVATE spdlog::spdlog)


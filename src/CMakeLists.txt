find_package(FluidSynth CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


set(SOURCE_FILES
  instrument.cpp
  voice_manager.cpp
  midi_creator.cpp
  compiler.cpp
  bpm_manager.cpp
  command.cpp
  player.cpp
)
add_library(synthlib STATIC ${SOURCE_FILES})

target_include_directories(synthlib PUBLIC ../include)
target_compile_features(synthlib PUBLIC cxx_std_17)
target_link_libraries(synthlib PRIVATE FluidSynth::libfluidsynth)
target_link_libraries(synthlib PRIVATE spdlog::spdlog)

add_executable(test_synthlib test_main.cpp ${SOURCE_FILES})
target_compile_definitions(test_synthlib PRIVATE CFG_TEST)
target_include_directories(test_synthlib PRIVATE ../include)
target_compile_features(test_synthlib PUBLIC cxx_std_17)
target_link_libraries(test_synthlib PRIVATE FluidSynth::libfluidsynth)
target_link_libraries(test_synthlib PRIVATE spdlog::spdlog)


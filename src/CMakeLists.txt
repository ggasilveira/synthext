find_package(FluidSynth CONFIG REQUIRED)


set(SOURCE_FILES
  instrument.cpp
  voice.cpp
  midi.cpp
  compiler.cpp
  bpm.cpp
  command.cpp
)
add_library(synthlib STATIC ${SOURCE_FILES})

target_include_directories(synthlib PUBLIC ../include)
target_compile_features(synthlib PUBLIC cxx_std_17)
target_link_libraries(synthlib PRIVATE FluidSynth::libfluidsynth)

add_executable(test_synthlib test_main.cpp ${SOURCE_FILES})
target_include_directories(test_synthlib PRIVATE ../include)
target_compile_features(test_synthlib PUBLIC cxx_std_17)
target_link_libraries(test_synthlib PRIVATE FluidSynth::libfluidsynth)


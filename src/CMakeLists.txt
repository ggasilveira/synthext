find_package(FLEX REQUIRED)
find_package(FluidSynth CONFIG REQUIRED)

# FLEX_TARGET(CommandScanner scanner.l "${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp")

add_library(synthlib STATIC text.cpp midi.cpp ${FLEX_CommandScanner_OUTPUTS})

target_include_directories(synthlib PUBLIC ../include)
target_compile_features(synthlib PUBLIC cxx_std_17)
target_link_libraries(synthlib PRIVATE FluidSynth::libfluidsynth)


add_library(synthlib STATIC text.cpp)
find_package(FluidSynth CONFIG REQUIRED)

target_include_directories(synthlib PUBLIC ../include)

target_compile_features(synthlib PUBLIC cxx_std_17)
target_link_libraries(synthlib PRIVATE FluidSynth::libfluidsynth)

# find_package(wxWidgets COMPONENTS base core)
find_package(spdlog CONFIG REQUIRED)

# Use fltk-config to get FLTK compilation flags
execute_process(
    COMMAND /opt/homebrew/bin/fltk-config --cxxflags
    OUTPUT_VARIABLE FLTK_CXXFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND /opt/homebrew/bin/fltk-config --ldflags
    OUTPUT_VARIABLE FLTK_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_executable(app app.cpp ui.cpp controls.cpp)
target_compile_features(app PRIVATE cxx_std_17)
set_target_properties(app PROPERTIES COMPILE_FLAGS "${FLTK_CXXFLAGS}")
set_target_properties(app PROPERTIES LINK_FLAGS "${FLTK_LDFLAGS}")

target_link_libraries(app PRIVATE synthlib)
target_link_libraries(app PRIVATE spdlog::spdlog)

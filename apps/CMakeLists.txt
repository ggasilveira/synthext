# find_package(wxWidgets COMPONENTS base core)
find_package(FLTK)
find_package(spdlog CONFIG REQUIRED)
add_executable(app app.cpp ui.cpp controls.cpp)
target_compile_features(app PRIVATE cxx_std_17)

target_link_libraries(app PRIVATE synthlib)
target_link_libraries(app PRIVATE fltk)
target_link_libraries(app PRIVATE spdlog::spdlog)

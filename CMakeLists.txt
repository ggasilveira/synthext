cmake_minimum_required(VERSION 3.28)

project(Synthext VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

#include(CTest)
include(FetchContent)

add_subdirectory(src)
add_subdirectory(apps)

# if(BUILD_TESTING)
#     add_subdirectory(tests)
# endif()


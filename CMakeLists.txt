cmake_minimum_required(VERSION 3.20)

project(Synthext VERSION 0.1 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(Synthext src/main.cpp)
target_include_directories(Synthext PRIVATE include)

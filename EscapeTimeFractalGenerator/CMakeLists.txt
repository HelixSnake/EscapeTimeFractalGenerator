cmake_minimum_required(VERSION 3.20)

set(CMAKE_INSTALL_PREFIX out)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(EscapeTimeFractalGenerator)

add_subdirectory(lib)
add_subdirectory(EscapeTimeFractalGenerator)

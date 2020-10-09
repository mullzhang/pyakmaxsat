cmake_minimum_required(VERSION 3.11)
project(cxxakmaxsat)

set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_FLAGS "-std=c++14 -O4")
set(CMAKE_CXX_FLAGS "-std=c++14 -O4 -fomit-frame-pointer -funroll-loops")

include(external/pybind11.cmake)
include_directories(akmaxsat_1.1)

add_subdirectory(src)

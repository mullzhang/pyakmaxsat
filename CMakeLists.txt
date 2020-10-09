cmake_minimum_required(VERSION 3.11)
project(cxxakmaxsat)

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -fomit-frame-pointer -funroll-loops -DNDEBUG")
set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -fomit-frame-pointer -funroll-loops -DFUIP -DCALC_MH -DRBFS -DNDEBUG -DFORCE_LS")

include(external/pybind11.cmake)
include_directories(akmaxsat_1.1)

add_subdirectory(src)

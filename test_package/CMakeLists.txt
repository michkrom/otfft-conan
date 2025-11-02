cmake_minimum_required(VERSION 3.15)
project(otfft_test)

find_package(otfft REQUIRED)

add_executable(test src/test.cpp)
target_link_libraries(test otfft::otfft)

set_property(TARGET test PROPERTY CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.15)
project(otfft_example)

find_package(otfft REQUIRED)

# Create executable
add_executable(otfft_example main.cpp)

# Link libraries (mainly for include paths since OTFFT is header-only)
target_link_libraries(otfft_example otfft::otfft)

# Set C++ standard
set_property(TARGET otfft_example PROPERTY CXX_STANDARD 11)
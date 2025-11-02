cmake_minimum_required(VERSION 3.1)
project(otfft_example)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Create executable
add_executable(otfft_example main.cpp)

# Link libraries (mainly for include paths since OTFFT is header-only)
target_link_libraries(otfft_example ${CONAN_LIBS})

# Set C++ standard
set_property(TARGET otfft_example PROPERTY CXX_STANDARD 11)
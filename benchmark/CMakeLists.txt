cmake_minimum_required(VERSION 3.1)
project(otfft_benchmark)

# Use Conan's cmake_find_package generator
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# Create executable
add_executable(perf_comparison perf_comparison.cpp)

# Set C++ standard
set_property(TARGET perf_comparison PROPERTY CXX_STANDARD 11)

# Check which libraries are available from Conan
set(HAVE_FFTW3 FALSE)
set(HAVE_OTFFT FALSE)
set(HAVE_MKL FALSE)
set(HAVE_KISSFFT FALSE)
set(HAVE_PFFFT FALSE)
set(HAVE_POCKETFFT FALSE)

# Find and link FFTW3 (always required - the standard/primary FFT library)
find_package(FFTW3 REQUIRED)
set(HAVE_FFTW3 TRUE)
target_link_libraries(perf_comparison FFTW3::FFTW3)
target_compile_definitions(perf_comparison PRIVATE HAVE_FFTW3)
message(STATUS "FFTW3: Enabled (via Conan) - PRIMARY FFT LIBRARY")

# Find and link OTFFT (now optional)
find_package(otfft)
if(otfft_FOUND)
    set(HAVE_OTFFT TRUE)
    target_link_libraries(perf_comparison otfft::otfft)
    target_compile_definitions(perf_comparison PRIVATE HAVE_OTFFT)
    message(STATUS "OTFFT: Enabled (via Conan)")
else()
    message(STATUS "OTFFT: Not available - enable with -o with_otfft=True")
endif()

# Find and link KissFFT
find_package(kissfft)
if(kissfft_FOUND)
    set(HAVE_KISSFFT TRUE)
    target_link_libraries(perf_comparison kissfft::kissfft)
    target_compile_definitions(perf_comparison PRIVATE HAVE_KISSFFT)
    message(STATUS "KissFFT: Enabled (via Conan)")
else()
    message(STATUS "KissFFT: Not available")
endif()

# Find and link PFFFT
find_package(pffft)
if(pffft_FOUND)
    set(HAVE_PFFFT TRUE)
    target_link_libraries(perf_comparison pffft::pffft)
    target_compile_definitions(perf_comparison PRIVATE HAVE_PFFFT)
    message(STATUS "PFFFT: Enabled (via Conan)")
else()
    message(STATUS "PFFFT: Not available")
endif()

# Find and link PocketFFT (header-only library)
find_package(pocketfft)
if(pocketfft_FOUND)
    set(HAVE_POCKETFFT TRUE)
    target_link_libraries(perf_comparison pocketfft::pocketfft)
    target_compile_definitions(perf_comparison PRIVATE HAVE_POCKETFFT)
    message(STATUS "PocketFFT: Enabled (via Conan)")
else()
    message(STATUS "PocketFFT: Not available")
endif()

# Find and link Intel MKL
find_package(intel-oneapi-mkl)
if(intel-oneapi-mkl_FOUND)
    set(HAVE_MKL TRUE)
    target_link_libraries(perf_comparison intel-oneapi-mkl::intel-oneapi-mkl)
    target_compile_definitions(perf_comparison PRIVATE HAVE_MKL)
    message(STATUS "Intel MKL: Enabled (via Conan)")
else()
    # Try to find system-installed Intel MKL
    set(MKL_SEARCH_PATHS
        $ENV{MKLROOT}
        /opt/intel/oneapi/mkl/latest
        /opt/intel/mkl
        /usr/local/intel/mkl
        /usr
        $ENV{HOME}/intel/oneapi/mkl/latest
    )
    
    find_path(MKL_INCLUDE_DIR 
        NAMES mkl.h
        PATHS ${MKL_SEARCH_PATHS}
        PATH_SUFFIXES include include/mkl
    )
    
    find_library(MKL_CORE_LIBRARY
        NAMES mkl_core
        PATHS ${MKL_SEARCH_PATHS}
        PATH_SUFFIXES lib lib/intel64 lib/x86_64-linux-gnu
    )
    
    find_library(MKL_INTEL_LP64_LIBRARY
        NAMES mkl_intel_lp64
        PATHS ${MKL_SEARCH_PATHS}
        PATH_SUFFIXES lib lib/intel64 lib/x86_64-linux-gnu
    )
    
    find_library(MKL_SEQUENTIAL_LIBRARY
        NAMES mkl_sequential
        PATHS ${MKL_SEARCH_PATHS}
        PATH_SUFFIXES lib lib/intel64 lib/x86_64-linux-gnu
    )
    
    if(MKL_INCLUDE_DIR AND MKL_CORE_LIBRARY AND MKL_INTEL_LP64_LIBRARY AND MKL_SEQUENTIAL_LIBRARY)
        set(HAVE_MKL TRUE)
        target_compile_definitions(perf_comparison PRIVATE HAVE_MKL)
        target_include_directories(perf_comparison PRIVATE ${MKL_INCLUDE_DIR})
        target_link_libraries(perf_comparison 
            ${MKL_INTEL_LP64_LIBRARY}
            ${MKL_SEQUENTIAL_LIBRARY}
            ${MKL_CORE_LIBRARY}
            pthread
            m
            dl
        )
        message(STATUS "Intel MKL: Enabled (system installation)")
        message(STATUS "  Include: ${MKL_INCLUDE_DIR}")
        message(STATUS "  Library: ${MKL_CORE_LIBRARY}")
    else()
        message(STATUS "Intel MKL: Not found")
    endif()
endif()

# Summary
message(STATUS "")
message(STATUS "=== Benchmark Libraries ===")
message(STATUS "  FFTW3:     YES (always)")
message(STATUS "  OTFFT:     ${HAVE_OTFFT}")
message(STATUS "  KissFFT:   ${HAVE_KISSFFT}")
message(STATUS "  PFFFT:     ${HAVE_PFFFT}")
message(STATUS "  PocketFFT: ${HAVE_POCKETFFT}")
message(STATUS "  Intel MKL: ${HAVE_MKL}")
message(STATUS "==========================")

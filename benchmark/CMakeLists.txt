cmake_minimum_required(VERSION 3.15)
project(otfft_benchmark)

find_package(otfft REQUIRED)

# Create executable
add_executable(perf_comparison perf_comparison.cpp)

# Link OTFFT (always required)
target_link_libraries(perf_comparison otfft::otfft)

# Set C++ standard
set_property(TARGET perf_comparison PROPERTY CXX_STANDARD 11)

# Find optional libraries
find_package(fftw)
find_package(kissfft)
find_package(pffft)
find_package(pocketfft)

# Check which libraries are available from Conan
set(HAVE_FFTW3 FALSE)
set(HAVE_MKL FALSE)
set(HAVE_KISSFFT FALSE)
set(HAVE_PFFFT FALSE)
set(HAVE_POCKETFFT FALSE)

# Check if FFTW3 is available
if(TARGET fftw::fftw)
    set(HAVE_FFTW3 TRUE)
    target_compile_definitions(perf_comparison PRIVATE HAVE_FFTW3)
    target_link_libraries(perf_comparison fftw::fftw)
    message(STATUS "FFTW3: Enabled (via Conan)")
else()
    message(STATUS "FFTW3: Not available - add fftw/3.3.10 to conanfile.txt")
endif()

# Check if KissFFT is available
if(TARGET kissfft::kissfft)
    set(HAVE_KISSFFT TRUE)
    target_compile_definitions(perf_comparison PRIVATE HAVE_KISSFFT)
    target_link_libraries(perf_comparison kissfft::kissfft)
    message(STATUS "KissFFT: Enabled (via Conan)")
else()
    message(STATUS "KissFFT: Not available")
endif()

# Check if PFFFT is available
if(TARGET pffft::pffft)
    set(HAVE_PFFFT TRUE)
    target_compile_definitions(perf_comparison PRIVATE HAVE_PFFFT)
    target_link_libraries(perf_comparison pffft::pffft)
    message(STATUS "PFFFT: Enabled (via Conan)")
else()
    message(STATUS "PFFFT: Not available")
endif()

# Check if PocketFFT is available
if(TARGET pocketfft::pocketfft)
    set(HAVE_POCKETFFT TRUE)
    target_compile_definitions(perf_comparison PRIVATE HAVE_POCKETFFT)
    target_link_libraries(perf_comparison pocketfft::pocketfft)
    message(STATUS "PocketFFT: Enabled (via Conan)")
else()
    message(STATUS "PocketFFT: Not available")
endif()

# Try to find MKL (Conan or system)
find_package(mkl)
if(TARGET mkl::mkl)
    set(HAVE_MKL TRUE)
    target_compile_definitions(perf_comparison PRIVATE HAVE_MKL)
    target_link_libraries(perf_comparison mkl::mkl)
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
message(STATUS "  OTFFT:     YES (always)")
message(STATUS "  FFTW3:     ${HAVE_FFTW3}")
message(STATUS "  KissFFT:   ${HAVE_KISSFFT}")
message(STATUS "  PFFFT:     ${HAVE_PFFFT}")
message(STATUS "  PocketFFT: ${HAVE_POCKETFFT}")
message(STATUS "  Intel MKL: ${HAVE_MKL}")
message(STATUS "==========================")

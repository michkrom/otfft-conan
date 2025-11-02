# OTFFT Conan Package

This is a Conan package for OTFFT (A high-speed FFT library using Stockham's algorithm and AVX/SSE).

## About OTFFT

OTFFT is a C++ library that provides high-performance Fast Fourier Transform implementations optimized for modern CPU architectures with AVX and SSE support.

## Usage

### Installing from source

1. Place the OTFFT source files in this directory, or the conanfile will attempt to download them automatically.

2. Create the package:
```bash
conan create . --build=missing
```

### Using in your project

Add OTFFT as a requirement in your `conanfile.txt`:
```
[requires]
otfft/11.5

[generators]
CMakeDeps
CMakeToolchain
```

Or in your `conanfile.py`:
```python
def requirements(self):
    self.requires("otfft/11.5")
```

### CMake Integration

```cmake
find_package(otfft REQUIRED)
target_link_libraries(your_target otfft::otfft)
```

## Package Information

- **Type**: Header-only library
- **License**: MIT
- **Homepage**: https://github.com/DEWETRON/OTFFT
- **Compiler Requirements**: C++11 compatible compiler with AVX/SSE support

## Build Requirements

- Modern C++ compiler (GCC, Clang, MSVC)
- AVX/SSE capable processor for optimal performance
- C++11 standard or higher

## Example Application

The `example/` directory contains a demonstration application showing how to use OTFFT for both Complex and Real FFT operations:

```bash
cd example
mkdir build && cd build
conan install ..
conan build ..
./bin/otfft_example
```

The example demonstrates:
- Complex FFT (forward and inverse)
- Real FFT (forward and inverse)
- Proper initialization and usage patterns
- Memory management with aligned allocations

## Performance Benchmark

The `benchmark/` directory contains a comprehensive performance comparison tool that benchmarks OTFFT against other popular FFT libraries:

### Included Libraries
- **OTFFT** (baseline) - High-performance FFT with AVX/SSE optimizations
- **FFTW3** - The widely-used "Fastest Fourier Transform in the West"
- **KissFFT** - Simple, portable FFT implementation
- **PFFFT** - "Pretty Fast FFT" optimized for small sizes
- **PocketFFT** - Header-only C++ FFT library
- **Intel MKL** - Intel's Math Kernel Library (optional, system installation)

### Building and Running the Benchmark

```bash
cd benchmark
mkdir build && cd build
conan install .. -s build_type=Release --build=missing
conan build ..
./bin/perf_comparison
```

### Sample Benchmark Results

The following results were obtained on a test system with GCC 13, testing FFT sizes from 128 to 8192 points with 10,000 iterations per test:

```
=====================================================================================================================
                                          FFT Performance Comparison Benchmark
=====================================================================================================================
Compiler: GCC 13.3.0 CPU: x86_64 Intel(R) Core(TM) Ultra 7 165H
Comparing: FFTW3 | OTFFT | Kiss | PFFFT | Pocket | MKL
Min test duration: 1 second(s)
Times shown in microseconds (μs)=====================================================================================================================

Min test duration: 1 second(s)
Times shown in microseconds (μs)
=====================================================================================================================

========== COMPLEX FFT TESTS ==========
Test Type       Size |  FFTW3 |  OTFFT | Ratio |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |    MKL | Ratio |
---------------------------------------------------------------------------------------------------------------------
Complex FFT      128 |   0.14 |   0.14 | 1.02x |   0.73 | 5.28x |   0.20 | 1.42x |   0.89 | 6.46x |   0.13 | 0.93x |
Complex FFT      256 |   0.30 |   0.40 | 1.32x |   1.43 | 4.71x |   0.37 | 1.23x |   1.59 | 5.25x |   0.28 | 0.91x |
Complex FFT      512 |   0.55 |   0.68 | 1.24x |   3.33 | 6.00x |   0.80 | 1.45x |   2.85 | 5.14x |   0.54 | 0.97x |
Complex FFT     1024 |   1.26 |   1.76 | 1.40x |   5.95 | 4.71x |   1.69 | 1.34x |   5.59 | 4.43x |   1.31 | 1.04x |
Complex FFT     2048 |   2.88 |   4.18 | 1.45x |  16.23 | 5.65x |   3.46 | 1.20x |  11.84 | 4.12x |   3.11 | 1.08x |
Complex FFT     4096 |   7.46 |  10.45 | 1.40x |  27.51 | 3.69x |   7.89 | 1.06x |  43.57 | 5.84x |   6.47 | 0.87x |
Complex FFT     8192 |  13.64 |  26.18 | 1.92x |  69.90 | 5.12x |  18.43 | 1.35x |  82.29 | 6.03x |  16.72 | 1.23x |
Complex FFT    16384 |  29.78 |  51.12 | 1.72x | 131.03 | 4.40x |  38.10 | 1.28x | 168.45 | 5.66x |  30.96 | 1.04x |

========== REAL FFT TESTS ==========
Test Type       Size |  FFTW3 |  OTFFT | Ratio |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |    MKL | Ratio |
---------------------------------------------------------------------------------------------------------------------
Real FFT         128 |   0.09 |   0.13 | 1.38x |   0.33 | 3.50x |   0.11 | 1.19x |   0.76 | 8.16x |   0.09 | 0.96x |
Real FFT         256 |   0.18 |   0.31 | 1.72x |   1.01 | 5.64x |   0.23 | 1.27x |   1.54 | 8.62x |   0.21 | 1.15x |
Real FFT         512 |   0.34 |   0.56 | 1.62x |   1.85 | 5.40x |   0.48 | 1.40x |   2.57 | 7.49x |   0.36 | 1.05x |
Real FFT        1024 |   0.72 |   1.24 | 1.72x |   4.14 | 5.72x |   0.72 | 1.00x |   3.77 | 5.20x |   0.71 | 0.97x |
Real FFT        2048 |   1.54 |   2.68 | 1.74x |   6.86 | 4.45x |   1.70 | 1.10x |   7.58 | 4.92x |   1.63 | 1.06x |
Real FFT        4096 |   3.40 |   5.86 | 1.72x |  17.52 | 5.15x |   3.60 | 1.06x |  14.71 | 4.32x |   4.18 | 1.23x |
Real FFT        8192 |   8.64 |  14.33 | 1.66x |  32.50 | 3.76x |   9.25 | 1.07x |  33.72 | 3.91x |   9.70 | 1.12x |
Real FFT       16384 |  17.91 |  36.39 | 2.03x |  84.68 | 4.73x |  21.68 | 1.21x |  83.76 | 4.68x |  21.21 | 1.18x |
```

```
===================================================================================
                         FFT Performance Comparison Benchmark
===================================================================================
Compiler: GCC 14.2.0 CPU: aarch64 Raspberry Pi 5 Model B Rev 1.0
Comparing: FFTW3 | Kiss | PFFFT | Pocket
Min test duration: 1 second(s)
Times shown in microseconds (μs)
===================================================================================

========== COMPLEX FFT TESTS ==========
Test Type       Size |  FFTW3 |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |
-----------------------------------------------------------------------------------
Complex FFT      128 |   0.76 |   1.77 | 2.33x |   0.46 | 0.61x |   2.36 | 3.11x |
Complex FFT      256 |   1.85 |   3.25 | 1.76x |   0.88 | 0.48x |   3.45 | 1.87x |
Complex FFT      512 |   4.37 |   8.15 | 1.86x |   1.92 | 0.44x |   6.49 | 1.48x |
Complex FFT     1024 |  10.77 |  15.22 | 1.41x |   3.95 | 0.37x |  12.96 | 1.20x |
Complex FFT     2048 |  25.54 |  37.28 | 1.46x |   8.76 | 0.34x |  26.71 | 1.05x |
Complex FFT     4096 |  62.17 |  71.52 | 1.15x |  18.50 | 0.30x |  68.99 | 1.11x |
Complex FFT     8192 | 124.56 | 175.18 | 1.41x |  42.31 | 0.34x | 150.77 | 1.21x |
Complex FFT    16384 | 343.40 | 356.81 | 1.04x |  94.52 | 0.28x | 333.25 | 0.97x |

========== REAL FFT TESTS ==========
Test Type       Size |  FFTW3 |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |
-----------------------------------------------------------------------------------
Real FFT         128 |   0.51 |   0.88 | 1.72x |   0.26 | 0.52x |   1.78 | 3.48x |
Real FFT         256 |   1.01 |   2.14 | 2.13x |   0.46 | 0.45x |   2.85 | 2.83x |
Real FFT         512 |   2.11 |   3.99 | 1.89x |   0.84 | 0.40x |   5.19 | 2.46x |
Real FFT        1024 |   4.60 |   9.65 | 2.10x |   1.65 | 0.36x |   8.97 | 1.95x |
Real FFT        2048 |  10.58 |  18.21 | 1.72x |   3.77 | 0.36x |  18.63 | 1.76x |
Real FFT        4096 |  27.27 |  43.40 | 1.59x |   7.78 | 0.29x |  36.79 | 1.35x |
Real FFT        8192 |  55.03 |  83.52 | 1.52x |  16.85 | 0.31x |  88.42 | 1.61x |
Real FFT       16384 | 148.38 | 201.22 | 1.36x |  41.76 | 0.28x | 198.62 | 1.34x |

===================================================================================
Note: Ratio shows speedup relative to FFTW3
===================================================================================
```

**Note:** Ratio values show performance relative to OTFFT. Values < 1.0x indicate the compared library is faster than OTFFT for that particular size.

### Key Observations
- OTFFT shows competitive performance across most FFT sizes
- Real FFT operations are generally faster than Complex FFT
- Different libraries excel at different FFT sizes
- Intel MKL and PFFFT show particularly strong performance for smaller sizes
- Performance characteristics vary significantly with FFT size

## Notes

The conanfile automatically adds appropriate compiler flags for AVX/SSE optimization based on your compiler.

For best performance, ensure your target processor supports AVX instructions.

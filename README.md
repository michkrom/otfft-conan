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
Compiler: GCC 13.3.0
Comparing: OTFFT | FFTW3 | Kiss | PFFFT | Pocket | MKL
Min test duration: 1 second(s)
Times shown in microseconds (μs)
=====================================================================================================================

========== COMPLEX FFT TESTS ==========
Test Type       Size |  OTFFT |  FFTW3 | Ratio |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |    MKL | Ratio |
---------------------------------------------------------------------------------------------------------------------
Complex FFT      128 |   0.16 |   0.12 | 0.77x |   0.78 | 4.92x |   0.20 | 1.26x |   0.83 | 5.24x |   0.13 | 0.81x |
Complex FFT      256 |   0.31 |   0.23 | 0.75x |   1.22 | 3.92x |   0.41 | 1.31x |   1.46 | 4.70x |   0.25 | 0.81x |
Complex FFT      512 |   0.65 |   0.49 | 0.76x |   3.33 | 5.15x |   0.81 | 1.25x |   2.91 | 4.50x |   0.50 | 0.78x |
Complex FFT     1024 |   1.80 |   1.43 | 0.79x |   5.61 | 3.11x |   1.53 | 0.85x |   5.17 | 2.87x |   1.66 | 0.92x |
Complex FFT     2048 |   3.57 |   2.68 | 0.75x |  15.60 | 4.37x |   4.17 | 1.17x |  10.45 | 2.93x |   2.67 | 0.75x |
Complex FFT     4096 |   9.72 |   7.13 | 0.73x |  27.21 | 2.80x |   8.06 | 0.83x |  22.27 | 2.29x |   6.42 | 0.66x |
Complex FFT     8192 | 147.00 |  17.13 | 0.12x |  75.50 | 0.51x |  22.17 | 0.15x |  59.72 | 0.41x |  16.56 | 0.11x |
Complex FFT    16384 | 163.05 |  39.04 | 0.24x | 123.12 | 0.76x |  35.98 | 0.22x | 102.21 | 0.63x |  35.50 | 0.22x |

========== REAL FFT TESTS ==========
Test Type       Size |  OTFFT |  FFTW3 | Ratio |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |    MKL | Ratio |
---------------------------------------------------------------------------------------------------------------------
Real FFT         128 |   0.14 |   0.10 | 0.71x |   0.33 | 2.36x |   0.11 | 0.80x |   0.73 | 5.19x |   0.09 | 0.65x |
Real FFT         256 |   0.27 |   0.18 | 0.65x |   0.89 | 3.28x |   0.19 | 0.70x |   1.20 | 4.42x |   0.20 | 0.73x |
Real FFT         512 |   0.78 |   0.36 | 0.46x |   1.49 | 1.90x |   0.37 | 0.48x |   2.07 | 2.63x |   0.33 | 0.43x |
Real FFT        1024 |   1.37 |   0.75 | 0.55x |   3.80 | 2.78x |   0.72 | 0.53x |   5.34 | 3.90x |   0.89 | 0.65x |
Real FFT        2048 |   3.69 |   1.62 | 0.44x |   6.67 | 1.81x |   1.62 | 0.44x |   7.03 | 1.91x |   1.53 | 0.42x |
Real FFT        4096 |   6.43 |   3.92 | 0.61x |  17.62 | 2.74x |   3.26 | 0.51x |  15.14 | 2.35x |   3.93 | 0.61x |
Real FFT        8192 |  16.01 |   8.29 | 0.52x |  31.75 | 1.98x |   8.84 | 0.55x |  41.83 | 2.61x |  11.24 | 0.70x |
Real FFT       16384 | 169.11 |  21.97 | 0.13x |  80.18 | 0.47x |  19.41 | 0.11x |  69.03 | 0.41x |  19.99 | 0.12x |
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

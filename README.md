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
conan install ..
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./bin/perf_comparison
```

### Sample Benchmark Results

The following results were obtained on a test system with GCC 13, testing FFT sizes from 128 to 8192 points with 10,000 iterations per test:

```
========================================================================================================
                            FFT Performance Comparison Benchmark
========================================================================================================
Comparing: OTFFT | FFTW3 | KissFFT | PFFFT | PocketFFT | Intel MKL
Iterations per test: 10000
Times shown in microseconds (μs)
========================================================================================================

========== COMPLEX FFT TESTS ==========

Test Type       Size |  OTFFT |  FFTW3 | Ratio |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |    MKL | Ratio
-------------------------------------------------------------------------------------------------------------------
Complex FFT      128 |   0.73 |   0.49 | 0.67x |   1.36 | 1.86x |   0.39 | 0.54x |   1.60 | 2.18x |   0.22 | 0.30x
Complex FFT      256 |   1.59 |   1.59 | 1.00x |   2.55 | 1.60x |   0.78 | 0.49x |   2.39 | 1.50x |   0.32 | 0.20x
Complex FFT      512 |   1.99 |   2.89 | 1.45x |   5.30 | 2.66x |   1.27 | 0.64x |   3.64 | 1.83x |   0.88 | 0.44x
Complex FFT     1024 |   4.80 |   7.19 | 1.50x |   7.80 | 1.62x |   2.34 | 0.49x |   9.69 | 2.02x |   2.12 | 0.44x
Complex FFT     2048 |  11.76 |  13.99 | 1.19x |  20.59 | 1.75x |   4.49 | 0.38x |  16.76 | 1.42x |   4.37 | 0.37x
Complex FFT     4096 |  22.24 |  42.45 | 1.91x |  35.59 | 1.60x |  10.29 | 0.46x |  29.19 | 1.31x |  16.37 | 0.74x
Complex FFT     8192 | 189.20 | 129.15 | 0.68x | 100.10 | 0.53x |  23.49 | 0.12x |  70.82 | 0.37x |  18.99 | 0.10x

========== REAL FFT TESTS ==========

Test Type       Size |  OTFFT |  FFTW3 | Ratio |   Kiss | Ratio |  PFFFT | Ratio | Pocket | Ratio |    MKL | Ratio
-------------------------------------------------------------------------------------------------------------------
Real FFT         128 |   0.14 |   0.22 | 1.56x |   0.49 | 3.43x |   0.16 | 1.13x |   1.16 | 8.05x |   0.10 | 0.72x
Real FFT         256 |   0.35 |   0.60 | 1.70x |   1.23 | 3.52x |   0.34 | 0.96x |   1.94 | 5.54x |   0.25 | 0.71x
Real FFT         512 |   0.65 |   1.11 | 1.70x |   1.99 | 3.04x |   0.58 | 0.88x |   2.95 | 4.50x |   0.40 | 0.61x
Real FFT        1024 |   1.23 |   2.84 | 2.31x |   5.70 | 4.64x |   1.03 | 0.84x |   5.22 | 4.24x |   1.11 | 0.91x
Real FFT        2048 |   3.62 |   6.94 | 1.92x |   9.44 | 2.61x |   2.33 | 0.64x |   9.65 | 2.67x |   2.24 | 0.62x
Real FFT        4096 |   8.57 |  13.14 | 1.53x |  22.33 | 2.61x |   4.26 | 0.50x |  20.96 | 2.45x |   5.92 | 0.69x
Real FFT        8192 |  23.88 |  40.16 | 1.68x |  42.76 | 1.79x |  10.97 | 0.46x |  43.82 | 1.83x |  13.36 | 0.56x
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
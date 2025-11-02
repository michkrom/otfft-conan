# OTFFT Performance Comparison Benchmark

This benchmark compares the performance of OTFFT against other popular FFT libraries:
- **FFTW3** - The Fastest Fourier Transform in the West
- **Intel MKL** - Intel Math Kernel Library

## Building

### Prerequisites

There is no intel-mkl conan so to include it in benchmark it should be installed in the OS.

### Build Instructions

```bash
mkdir build && cd build
conan install ..
conan build ..
```

## Running

```bash
./bin/perf_comparison
```

## What It Tests

The benchmark runs the following tests:
- **Complex FFT**: Forward FFT on complex data
- **Real FFT**: Forward FFT on real-valued data

Test sizes: 128, 256, 512, 1024, 2048, 4096, 8192, 16384

Each test is run for at least 1 second and averaged.

## Output

The benchmark will show:
1. Absolute time for each library
2. Speed ratio compared to OTFFT
3. A value > 1.0 means OTFFT is faster

## Notes

- The benchmark will automatically detect which libraries are available
- If FFTW3 or MKL are not found, only available libraries will be compared
- All tests use the same input data for fair comparison
- Performance is averaged over 1s run time for each case

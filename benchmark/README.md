# OTFFT Performance Comparison Benchmark

This benchmark compares the performance of OTFFT against other popular FFT libraries:
- **FFTW3** - The Fastest Fourier Transform in the West
- **Intel MKL** - Intel Math Kernel Library

## Building

### Prerequisites

1. OTFFT package must be installed:
   ```bash
   cd ..
   conan export . otfft/11.5@
   ```

2. (Optional) Install FFTW3:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libfftw3-dev
   
   # macOS
   brew install fftw
   ```

3. (Optional) Install Intel MKL:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install intel-mkl
   
   # Or download from Intel's website
   ```

### Build Instructions

```bash
mkdir build && cd build
conan install .. --install-folder=.
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Running

```bash
./bin/perf_comparison
```

## What It Tests

The benchmark runs the following tests:
- **Complex FFT**: Forward FFT on complex data
- **Real FFT**: Forward FFT on real-valued data

Test sizes: 128, 256, 512, 1024, 2048, 4096, 8192

Each test is run 1000 times and the average time is reported in microseconds (μs).

## Output

The benchmark will show:
1. Absolute time for each library
2. Speed ratio compared to OTFFT
3. A value > 1.0 means OTFFT is faster

## Notes

- The benchmark will automatically detect which libraries are available
- If FFTW3 or MKL are not found, only available libraries will be compared
- All tests use the same input data for fair comparison
- Times are averaged over 1000 iterations for accuracy

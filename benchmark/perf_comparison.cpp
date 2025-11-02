#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <string>

// OTFFT headers
#include "otfft.h"
#include "otfft_fwd.h"

// FFTW3 headers
#ifdef HAVE_FFTW3
#include <fftw3.h>
#endif

// Intel MKL headers
#ifdef HAVE_MKL
#include <mkl_dfti.h>
#endif

// KissFFT headers
#ifdef HAVE_KISSFFT
#include "kiss_fft.h"
#include "kiss_fftr.h"
#endif

// PFFFT headers
#ifdef HAVE_PFFFT
#include "pffft.h"
#endif

// PocketFFT headers
#ifdef HAVE_POCKETFFT
#include "pocketfft_hdronly.h"
#endif

using namespace std;
using complex_t = OTFFT::complex_t;

// Performance test helper
template<typename Func>
double measure_time_us(Func&& func, int iterations = 10000) {
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, micro> duration = end - start;
    return duration.count() / iterations;
}

// OTFFT Complex FFT test
double test_otfft_complex(int N, int iterations = 10000) {
    vector<complex_t> data(N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = complex_t(sin(t) + 0.5 * sin(5.0 * t), 0.0);
    }
    
    auto fft = OTFFT::Factory::createComplexFFT(N);
    
    double time = measure_time_us([&]() {
        fft->fwd(data.data());
    }, iterations);
    
    return time;
}

// OTFFT Real FFT test
double test_otfft_real(int N, int iterations = 10000) {
    vector<double> realInput(N);
    vector<complex_t> spectrum(N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        realInput[i] = sin(t) + 0.5 * sin(5.0 * t);
    }
    
    auto rfft = OTFFT::Factory::createRealFFT(N);
    
    double time = measure_time_us([&]() {
        rfft->fwd(realInput.data(), spectrum.data());
    }, iterations);
    
    return time;
}

#ifdef HAVE_FFTW3
// FFTW3 Complex FFT test
double test_fftw3_complex(int N, int iterations = 10000) {
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        in[i][0] = sin(t) + 0.5 * sin(5.0 * t);
        in[i][1] = 0.0;
    }
    
    // Use FFTW_MEASURE for optimized plans (planning takes longer but execution is faster)
    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_MEASURE);
    
    double time = measure_time_us([&]() {
        fftw_execute(plan);
    }, iterations);
    
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    
    return time;
}

// FFTW3 Real FFT test
double test_fftw3_real(int N, int iterations = 10000) {
    double* in = (double*)fftw_malloc(sizeof(double) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (N/2 + 1));
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        in[i] = sin(t) + 0.5 * sin(5.0 * t);
    }
    
    // Use FFTW_MEASURE for optimized plans (planning takes longer but execution is faster)
    fftw_plan plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
    
    double time = measure_time_us([&]() {
        fftw_execute(plan);
    }, iterations);
    
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    
    return time;
}
#endif

#ifdef HAVE_MKL
// Intel MKL Complex FFT test
double test_mkl_complex(int N, int iterations = 10000) {
    vector<complex<double>> data(N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = complex<double>(sin(t) + 0.5 * sin(5.0 * t), 0.0);
    }
    
    DFTI_DESCRIPTOR_HANDLE handle = nullptr;
    DftiCreateDescriptor(&handle, DFTI_DOUBLE, DFTI_COMPLEX, 1, N);
    DftiSetValue(handle, DFTI_PLACEMENT, DFTI_INPLACE);
    DftiCommitDescriptor(handle);
    
    double time = measure_time_us([&]() {
        DftiComputeForward(handle, data.data());
    }, iterations);
    
    DftiFreeDescriptor(&handle);
    
    return time;
}

// Intel MKL Real FFT test
double test_mkl_real(int N, int iterations = 10000) {
    // For in-place real FFT, need N+2 doubles (to hold N/2+1 complex values)
    vector<double> data(N + 2, 0.0);
    
    // Initialize data (only first N elements)
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = sin(t) + 0.5 * sin(5.0 * t);
    }
    
    DFTI_DESCRIPTOR_HANDLE handle = nullptr;
    DftiCreateDescriptor(&handle, DFTI_DOUBLE, DFTI_REAL, 1, N);
    DftiSetValue(handle, DFTI_PLACEMENT, DFTI_INPLACE);
    DftiSetValue(handle, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
    DftiCommitDescriptor(handle);
    
    double time = measure_time_us([&]() {
        DftiComputeForward(handle, data.data());
    }, iterations);
    
    DftiFreeDescriptor(&handle);
    
    return time;
}
#endif

#ifdef HAVE_KISSFFT
// KissFFT Complex FFT test
double test_kissfft_complex(int N, int iterations = 10000) {
    vector<kiss_fft_cpx> in(N);
    vector<kiss_fft_cpx> out(N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        in[i].r = sin(t) + 0.5 * sin(5.0 * t);
        in[i].i = 0.0;
    }
    
    kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, nullptr, nullptr);
    
    double time = measure_time_us([&]() {
        kiss_fft(cfg, in.data(), out.data());
    }, iterations);
    
    kiss_fft_free(cfg);
    
    return time;
}

// KissFFT Real FFT test
double test_kissfft_real(int N, int iterations = 10000) {
    vector<kiss_fft_scalar> in(N);
    vector<kiss_fft_cpx> out(N/2 + 1);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        in[i] = sin(t) + 0.5 * sin(5.0 * t);
    }
    
    kiss_fftr_cfg cfg = kiss_fftr_alloc(N, 0, nullptr, nullptr);
    
    double time = measure_time_us([&]() {
        kiss_fftr(cfg, in.data(), out.data());
    }, iterations);
    
    kiss_fftr_free(cfg);
    
    return time;
}
#endif

#ifdef HAVE_PFFFT
// PFFFT Complex FFT test
double test_pffft_complex(int N, int iterations = 10000) {
    vector<float> data(N * 2); // interleaved real/imag
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i * 2] = sin(t) + 0.5 * sin(5.0 * t);
        data[i * 2 + 1] = 0.0;
    }
    
    PFFFT_Setup *setup = pffft_new_setup(N, PFFFT_COMPLEX);
    vector<float> work(N * 2);
    
    double time = measure_time_us([&]() {
        pffft_transform_ordered(setup, data.data(), data.data(), work.data(), PFFFT_FORWARD);
    }, iterations);
    
    pffft_destroy_setup(setup);
    
    return time;
}

// PFFFT Real FFT test
double test_pffft_real(int N, int iterations = 10000) {
    vector<float> data(N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = sin(t) + 0.5 * sin(5.0 * t);
    }
    
    PFFFT_Setup *setup = pffft_new_setup(N, PFFFT_REAL);
    vector<float> work(N);
    
    double time = measure_time_us([&]() {
        pffft_transform_ordered(setup, data.data(), data.data(), work.data(), PFFFT_FORWARD);
    }, iterations);
    
    pffft_destroy_setup(setup);
    
    return time;
}
#endif

#ifdef HAVE_POCKETFFT
// PocketFFT Complex FFT test
double test_pocketfft_complex(int N, int iterations = 10000) {
    vector<complex<double>> data(N);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = complex<double>(sin(t) + 0.5 * sin(5.0 * t), 0.0);
    }
    
    pocketfft::shape_t shape{static_cast<size_t>(N)};
    pocketfft::stride_t stride{static_cast<ptrdiff_t>(sizeof(complex<double>))};
    pocketfft::shape_t axes{0};
    
    double time = measure_time_us([&]() {
        pocketfft::c2c(shape, stride, stride, axes, pocketfft::FORWARD, 
                       data.data(), data.data(), 1.0);
    }, iterations);
    
    return time;
}

// PocketFFT Real FFT test
double test_pocketfft_real(int N, int iterations = 10000) {
    vector<double> data(N);
    vector<complex<double>> out(N/2 + 1);
    
    // Initialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = sin(t) + 0.5 * sin(5.0 * t);
    }
    
    pocketfft::shape_t shape{static_cast<size_t>(N)};
    pocketfft::stride_t stride_in{static_cast<ptrdiff_t>(sizeof(double))};
    pocketfft::stride_t stride_out{static_cast<ptrdiff_t>(sizeof(complex<double>))};
    pocketfft::shape_t axes{0};
    
    double time = measure_time_us([&]() {
        pocketfft::r2c(shape, stride_in, stride_out, axes, pocketfft::FORWARD,
                       data.data(), out.data(), 1.0);
    }, iterations);
    
    return time;
}
#endif

void print_header() {
    cout << "========================================================================================================" << endl;
    cout << "                            FFT Performance Comparison Benchmark" << endl;
    cout << "========================================================================================================" << endl;
    
    // Print compiler information
    cout << "Compiler: ";
#if defined(__GNUC__) && !defined(__clang__)
    cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined(__clang__)
    cout << "Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__;
#elif defined(_MSC_VER)
    cout << "MSVC " << _MSC_VER;
#else
    cout << "Unknown";
#endif
    cout << endl;
    
    cout << "Comparing: OTFFT";
#ifdef HAVE_FFTW3
    cout << " | FFTW3";
#endif
#ifdef HAVE_KISSFFT
    cout << " | KissFFT";
#endif
#ifdef HAVE_PFFFT
    cout << " | PFFFT";
#endif
#ifdef HAVE_POCKETFFT
    cout << " | PocketFFT";
#endif
#ifdef HAVE_MKL
    cout << " | Intel MKL";
#endif
    cout << endl;
    cout << "Iterations per test: 10000" << endl;
    cout << "Times shown in microseconds (μs)" << endl;
    cout << "========================================================================================================" << endl;
}

void print_comparison_row(const string& label, int N, double otfft_time, 
                         double fftw_time = -1, double kissfft_time = -1, 
                         double pffft_time = -1, double pocketfft_time = -1, 
                         double mkl_time = -1) {
    cout << left << setw(15) << label;
    cout << right << setw(5) << N << " |";
    cout << fixed << setprecision(2) << setw(7) << otfft_time;
    
#ifdef HAVE_FFTW3
    if (fftw_time >= 0) {
        cout << " |" << setw(7) << fftw_time;
        double speedup = fftw_time / otfft_time;
        cout << " |" << setw(5) << speedup << "x";
    } else {
        cout << " |" << setw(7) << "N/A" << " |" << setw(5) << "N/A";
    }
#endif

#ifdef HAVE_KISSFFT
    if (kissfft_time >= 0) {
        cout << " |" << setw(7) << kissfft_time;
        double speedup = kissfft_time / otfft_time;
        cout << " |" << setw(5) << speedup << "x";
    } else {
        cout << " |" << setw(7) << "N/A" << " |" << setw(5) << "N/A";
    }
#endif

#ifdef HAVE_PFFFT
    if (pffft_time >= 0) {
        cout << " |" << setw(7) << pffft_time;
        double speedup = pffft_time / otfft_time;
        cout << " |" << setw(5) << speedup << "x";
    } else {
        cout << " |" << setw(7) << "N/A" << " |" << setw(5) << "N/A";
    }
#endif

#ifdef HAVE_POCKETFFT
    if (pocketfft_time >= 0) {
        cout << " |" << setw(7) << pocketfft_time;
        double speedup = pocketfft_time / otfft_time;
        cout << " |" << setw(5) << speedup << "x";
    } else {
        cout << " |" << setw(7) << "N/A" << " |" << setw(5) << "N/A";
    }
#endif

#ifdef HAVE_MKL
    if (mkl_time >= 0) {
        cout << " |" << setw(7) << mkl_time;
        double speedup = mkl_time / otfft_time;
        cout << " |" << setw(5) << speedup << "x";
    } else {
        cout << " |" << setw(7) << "N/A" << " |" << setw(5) << "N/A";
    }
#endif
    
    cout << endl;
}

void print_table_header() {
    cout << "\n" << left << setw(15) << "Test Type";
    cout << right << setw(5) << "Size" << " |";
    cout << setw(7) << "OTFFT";
    
#ifdef HAVE_FFTW3
    cout << " |" << setw(7) << "FFTW3" << " |" << setw(6) << "Ratio";
#endif

#ifdef HAVE_KISSFFT
    cout << " |" << setw(7) << "Kiss" << " |" << setw(6) << "Ratio";
#endif

#ifdef HAVE_PFFFT
    cout << " |" << setw(7) << "PFFFT" << " |" << setw(6) << "Ratio";
#endif

#ifdef HAVE_POCKETFFT
    cout << " |" << setw(7) << "Pocket" << " |" << setw(6) << "Ratio";
#endif

#ifdef HAVE_MKL
    cout << " |" << setw(7) << "MKL" << " |" << setw(6) << "Ratio";
#endif
    
    cout << endl;
    
    // Calculate separator line width dynamically
    int width = 15 + 5 + 7 + 3;  // "Test Type" (15) + "Size" (5) + "OTFFT" (7) + separators (3)
#ifdef HAVE_FFTW3
    width += 7 + 6 + 4;  // FFTW3 (7) + Ratio (6) + separators (4 for " | x")
#endif
#ifdef HAVE_KISSFFT
    width += 7 + 6 + 4;  // Kiss (7) + Ratio (6) + separators (4)
#endif
#ifdef HAVE_PFFFT
    width += 7 + 6 + 4;  // PFFFT (7) + Ratio (6) + separators (4)
#endif
#ifdef HAVE_POCKETFFT
    width += 7 + 6 + 4;  // Pocket (7) + Ratio (6) + separators (4)
#endif
#ifdef HAVE_MKL
    width += 7 + 6 + 4;  // MKL (7) + Ratio (6) + separators (4)
#endif
    
    cout << string(width, '-') << endl;
}

int main() {
    print_header();
    
    vector<int> sizes = {128, 256, 512, 1024, 2048, 4096, 8192};
    
    // Complex FFT Tests
    cout << "\n========== COMPLEX FFT TESTS ==========" << endl;
    print_table_header();
    
    for (int N : sizes) {
        double otfft_time = test_otfft_complex(N);
        
#ifdef HAVE_FFTW3
        double fftw_time = test_fftw3_complex(N);
#else
        double fftw_time = -1;
#endif

#ifdef HAVE_KISSFFT
        double kissfft_time = test_kissfft_complex(N);
#else
        double kissfft_time = -1;
#endif

#ifdef HAVE_PFFFT
        double pffft_time = test_pffft_complex(N);
#else
        double pffft_time = -1;
#endif

#ifdef HAVE_POCKETFFT
        double pocketfft_time = test_pocketfft_complex(N);
#else
        double pocketfft_time = -1;
#endif

#ifdef HAVE_MKL
        double mkl_time = test_mkl_complex(N);
#else
        double mkl_time = -1;
#endif
        
        print_comparison_row("Complex FFT", N, otfft_time, fftw_time, kissfft_time, 
                           pffft_time, pocketfft_time, mkl_time);
    }
    
    // Real FFT Tests
    cout << "\n========== REAL FFT TESTS ==========" << endl;
    print_table_header();
    
    for (int N : sizes) {
        double otfft_time = test_otfft_real(N);
        
#ifdef HAVE_FFTW3
        double fftw_time = test_fftw3_real(N);
#else
        double fftw_time = -1;
#endif

#ifdef HAVE_KISSFFT
        double kissfft_time = test_kissfft_real(N);
#else
        double kissfft_time = -1;
#endif

#ifdef HAVE_PFFFT
        double pffft_time = test_pffft_real(N);
#else
        double pffft_time = -1;
#endif

#ifdef HAVE_POCKETFFT
        double pocketfft_time = test_pocketfft_real(N);
#else
        double pocketfft_time = -1;
#endif

#ifdef HAVE_MKL
        double mkl_time = test_mkl_real(N);
#else
        double mkl_time = -1;
#endif
        
        print_comparison_row("Real FFT", N, otfft_time, fftw_time, kissfft_time,
                           pffft_time, pocketfft_time, mkl_time);
    }
    
    cout << "\n========================================================================================================" << endl;
    cout << "Benchmark completed!" << endl;
    cout << "Note: Ratio shows speedup relative to OTFFT (higher is better for OTFFT)" << endl;
    cout << "========================================================================================================" << endl;
    
    return 0;
}

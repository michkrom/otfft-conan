#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
#include <iomanip>
#include <cmath>

// Include OTFFT headers
#include "otfft.h"
#include "otfft_fwd.h"

using namespace std;
using complex_t = OTFFT::complex_t;

// Performance test helper
template<typename Func>
double measure_time(Func&& func, int iterations = 1000) {
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, micro> duration = end - start;
    return duration.count() / iterations;
}

// Test complex FFT
void test_complex_fft(int N) {
    cout << "\n--- Testing Complex FFT (size: " << N << ") ---" << endl;
    
    // Allocate aligned memory for OTFFT
    vector<complex_t> data(N);
    
    // Initialize with test signal (sum of sinusoids)
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = complex_t(sin(t) + 0.5 * sin(5.0 * t), 0.0);
    }
    
    // Create FFT object using factory
    auto fft = OTFFT::Factory::createComplexFFT(N);
    
    // Measure forward FFT (in-place)
    double fwd_time = measure_time([&]() {
        fft->fwd(data.data());
    });
    
    // Reinitialize data
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = complex_t(sin(t) + 0.5 * sin(5.0 * t), 0.0);
    }
    
    // Measure inverse FFT (in-place)
    fft->fwd(data.data()); // Forward first
    double inv_time = measure_time([&]() {
        fft->inv(data.data());
    });
    
    cout << "  Forward FFT: " << fixed << setprecision(2) << fwd_time << " μs" << endl;
    cout << "  Inverse FFT: " << fixed << setprecision(2) << inv_time << " μs" << endl;
    cout << "  Total time:  " << fixed << setprecision(2) << (fwd_time + inv_time) << " μs" << endl;
}

// Test real FFT
void test_real_fft(int N) {
    cout << "\n--- Testing Real FFT (size: " << N << ") ---" << endl;
    
    // Allocate memory for real data and spectrum
    // OTFFT RealFFT uses in-place transforms, so we need N complex values
    vector<complex_t> data(N);
    
    // Initialize with test signal (real values only)
    for (int i = 0; i < N; ++i) {
        double t = 2.0 * M_PI * i / N;
        data[i] = complex_t(sin(t) + 0.5 * sin(5.0 * t), 0.0);
    }
    
    // Create Real FFT object
    auto rfft = OTFFT::Factory::createRealFFT(N);
    
    // Prepare separate input/output buffers for real data
    vector<double> realInput(N);
    for (int i = 0; i < N; ++i) {
        realInput[i] = data[i].Re;  // OTFFT uses .Re and .Im members
    }
    
    // Measure forward real FFT
    double fwd_time = measure_time([&]() {
        rfft->fwd(realInput.data(), data.data());
    });
    
    // Measure inverse real FFT
    vector<double> realOutput(N);
    double inv_time = measure_time([&]() {
        rfft->inv(data.data(), realOutput.data());
    });
    
    cout << "  Forward RFFT: " << fixed << setprecision(2) << fwd_time << " μs" << endl;
    cout << "  Inverse RFFT: " << fixed << setprecision(2) << inv_time << " μs" << endl;
    cout << "  Total time:   " << fixed << setprecision(2) << (fwd_time + inv_time) << " μs" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "OTFFT Performance Test Suite" << endl;
    cout << "========================================" << endl;
    
    vector<int> sizes = {128, 256, 512, 1024, 2048};
    
    try {
        // Test Complex FFT for all sizes
        cout << "\n========== COMPLEX FFT TESTS ==========" << endl;
        for (int N : sizes) {
            test_complex_fft(N);
        }
        
        // Test Real FFT for all sizes
        cout << "\n========== REAL FFT TESTS ==========" << endl;
        for (int N : sizes) {
            test_real_fft(N);
        }
        
        cout << "\n========================================" << endl;
        cout << "All tests completed successfully!" << endl;
        cout << "========================================" << endl;
        
        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
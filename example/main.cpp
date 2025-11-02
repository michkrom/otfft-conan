#include <iostream>
#include <vector>
#include <cmath>

// Include OTFFT headers
#include "otfft.h"
#include "otfft_fwd.h"

using namespace std;
using complex_t = OTFFT::complex_t;

int main() {
    cout << "========================================" << endl;
    cout << "OTFFT Example Application" << endl;
    cout << "========================================" << endl;
    
    const int N = 1024;
    
    // Example 1: Complex FFT
    cout << "\nExample 1: Complex FFT (size: " << N << ")" << endl;
    {
        // Allocate data
        vector<complex_t> data(N);
        
        // Initialize with a test signal (sine wave)
        for (int i = 0; i < N; ++i) {
            double t = 2.0 * M_PI * i / N;
            data[i] = complex_t(sin(t) + 0.5 * sin(5.0 * t), 0.0);
        }
        
        // Create FFT object using factory
        auto fft = OTFFT::Factory::createComplexFFT(N);
        
        // Perform forward FFT (in-place)
        fft->fwd(data.data());
        cout << "  Forward FFT completed" << endl;
        
        // Display a few frequency components
        cout << "  First few frequency bins:" << endl;
        for (int i = 0; i < 5; ++i) {
            cout << "    Bin " << i << ": (" << data[i].Re << ", " << data[i].Im << ")" << endl;
        }
        
        // Perform inverse FFT
        fft->inv(data.data());
        cout << "  Inverse FFT completed" << endl;
        
        // Verify reconstruction (check first value)
        cout << "  Reconstructed signal (first value): (" << data[0].Re << ", " << data[0].Im << ")" << endl;
    }
    
    // Example 2: Real FFT
    cout << "\nExample 2: Real FFT (size: " << N << ")" << endl;
    {
        // Allocate memory for real data and spectrum
        // OTFFT RealFFT requires N complex values for spectrum
        vector<double> realInput(N);
        vector<complex_t> spectrum(N);
        vector<double> realOutput(N);
        
        // Initialize with test signal
        for (int i = 0; i < N; ++i) {
            double t = 2.0 * M_PI * i / N;
            realInput[i] = sin(t) + 0.5 * sin(5.0 * t);
        }
        
        // Create Real FFT object
        auto rfft = OTFFT::Factory::createRealFFT(N);
        
        // Perform forward real FFT
        rfft->fwd(realInput.data(), spectrum.data());
        cout << "  Forward RFFT completed" << endl;
        
        // Display a few frequency components
        cout << "  First few frequency bins:" << endl;
        for (int i = 0; i < 5; ++i) {
            cout << "    Bin " << i << ": (" << spectrum[i].Re << ", " << spectrum[i].Im << ")" << endl;
        }
        
        // Perform inverse real FFT
        rfft->inv(spectrum.data(), realOutput.data());
        cout << "  Inverse RFFT completed" << endl;
        
        // Verify reconstruction
        cout << "  Reconstructed signal (first value): " << realOutput[0] << endl;
    }
    
    cout << "\n========================================" << endl;
    cout << "All examples run successfully!" << endl;
    cout << "========================================" << endl;
    
    return 0;
}
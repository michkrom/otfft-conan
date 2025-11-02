#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <string>
#include <memory>
#include <map>

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

// Abstract base class for FFT benchmarks
class FFTBenchmark {
protected:
    int size_;
    double min_duration_seconds_;
    string name_;
    int actual_iterations_;  // Store actual number of iterations performed
    
public:
    FFTBenchmark(const string& name, int size, double min_duration_seconds = 1.0) 
        : name_(name), size_(size), min_duration_seconds_(min_duration_seconds), actual_iterations_(0) {}
    
    virtual ~FFTBenchmark() {}
    
    // Setup phase (not timed) - allocate memory, create plans, etc.
    virtual void setup() = 0;
    
    // Execute one FFT (will be called in timing loop)
    virtual void execute() = 0;
    
    // Cleanup (not timed)
    virtual void cleanup() = 0;
    
    // Run the benchmark and return average time in microseconds
    // Runs for at least min_duration_seconds
    double run() {
        setup();
        
        int iterations = 0;
        auto start = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration<double>(0);
        
        // Run for at least the minimum duration
        do {
            execute();
            iterations++;
            elapsed = chrono::high_resolution_clock::now() - start;
        } while (elapsed.count() < min_duration_seconds_);
        
        auto end = chrono::high_resolution_clock::now();
        cleanup();
        
        actual_iterations_ = iterations;
        chrono::duration<double, micro> duration = end - start;
        return duration.count() / iterations;
    }
    
    string getName() const { return name_; }
    int getSize() const { return size_; }
    int getActualIterations() const { return actual_iterations_; }
};

// OTFFT Complex FFT Benchmark
class OTFFTComplexBenchmark : public FFTBenchmark {
private:
    vector<OTFFT::complex_t> data_;
    OTFFT::ComplexFFTPtr fft_;
    
public:
    OTFFTComplexBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("OTFFT", size, min_duration) {}
    
    void setup() override {
        data_.resize(size_);
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i] = OTFFT::complex_t(sin(t) + 0.5 * sin(5.0 * t), 0.0);
        }
        fft_ = OTFFT::Factory::createComplexFFT(size_);
    }
    
    void execute() override {
        fft_->fwd(data_.data());
    }
    
    void cleanup() override {
        fft_.reset();
        data_.clear();
    }
};

// OTFFT Real FFT Benchmark
class OTFFTRealBenchmark : public FFTBenchmark {
private:
    vector<double> data_;
    vector<OTFFT::complex_t> spectrum_;
    OTFFT::RealFFTPtr rfft_;
    
public:
    OTFFTRealBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("OTFFT", size, min_duration) {}
    
    void setup() override {
        data_.resize(size_);
        spectrum_.resize(size_);
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i] = sin(t) + 0.5 * sin(5.0 * t);
        }
        rfft_ = OTFFT::Factory::createRealFFT(size_);
    }
    
    void execute() override {
        rfft_->fwd(data_.data(), spectrum_.data());
    }
    
    void cleanup() override {
        rfft_.reset();
        data_.clear();
        spectrum_.clear();
    }
};

#ifdef HAVE_FFTW3
// FFTW3 Complex FFT Benchmark
class FFTW3ComplexBenchmark : public FFTBenchmark {
private:
    fftw_complex* in_;
    fftw_complex* out_;
    fftw_plan plan_;
    
public:
    FFTW3ComplexBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("FFTW3", size, min_duration), in_(nullptr), out_(nullptr), plan_(nullptr) {}
    
    void setup() override {
        in_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size_);
        out_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size_);
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            in_[i][0] = sin(t) + 0.5 * sin(5.0 * t);
            in_[i][1] = 0.0;
        }
        
        plan_ = fftw_plan_dft_1d(size_, in_, out_, FFTW_FORWARD, FFTW_MEASURE);
    }
    
    void execute() override {
        fftw_execute(plan_);
    }
    
    void cleanup() override {
        if (plan_) fftw_destroy_plan(plan_);
        if (in_) fftw_free(in_);
        if (out_) fftw_free(out_);
    }
};

// FFTW3 Real FFT Benchmark
class FFTW3RealBenchmark : public FFTBenchmark {
private:
    double* in_;
    fftw_complex* out_;
    fftw_plan plan_;
    
public:
    FFTW3RealBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("FFTW3", size, min_duration), in_(nullptr), out_(nullptr), plan_(nullptr) {}
    
    void setup() override {
        in_ = (double*)fftw_malloc(sizeof(double) * size_);
        out_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (size_/2 + 1));
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            in_[i] = sin(t) + 0.5 * sin(5.0 * t);
        }
        
        plan_ = fftw_plan_dft_r2c_1d(size_, in_, out_, FFTW_MEASURE);
    }
    
    void execute() override {
        fftw_execute(plan_);
    }
    
    void cleanup() override {
        if (plan_) fftw_destroy_plan(plan_);
        if (in_) fftw_free(in_);
        if (out_) fftw_free(out_);
    }
};
#endif

#ifdef HAVE_MKL
// Intel MKL Complex FFT Benchmark
class MKLComplexBenchmark : public FFTBenchmark {
private:
    vector<complex<double>> data_;
    DFTI_DESCRIPTOR_HANDLE handle_;
    
public:
    MKLComplexBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("MKL", size, min_duration), handle_(nullptr) {}
    
    void setup() override {
        data_.resize(size_);
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i] = complex<double>(sin(t) + 0.5 * sin(5.0 * t), 0.0);
        }
        
        DftiCreateDescriptor(&handle_, DFTI_DOUBLE, DFTI_COMPLEX, 1, size_);
        DftiSetValue(handle_, DFTI_PLACEMENT, DFTI_INPLACE);
        DftiCommitDescriptor(handle_);
    }
    
    void execute() override {
        DftiComputeForward(handle_, data_.data());
    }
    
    void cleanup() override {
        if (handle_) DftiFreeDescriptor(&handle_);
        data_.clear();
    }
};

// Intel MKL Real FFT Benchmark
class MKLRealBenchmark : public FFTBenchmark {
private:
    vector<double> data_;
    DFTI_DESCRIPTOR_HANDLE handle_;
    
public:
    MKLRealBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("MKL", size, min_duration), handle_(nullptr) {}
    
    void setup() override {
        data_.resize(size_ + 2, 0.0);
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i] = sin(t) + 0.5 * sin(5.0 * t);
        }
        
        DftiCreateDescriptor(&handle_, DFTI_DOUBLE, DFTI_REAL, 1, size_);
        DftiSetValue(handle_, DFTI_PLACEMENT, DFTI_INPLACE);
        DftiSetValue(handle_, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
        DftiCommitDescriptor(handle_);
    }
    
    void execute() override {
        DftiComputeForward(handle_, data_.data());
    }
    
    void cleanup() override {
        if (handle_) DftiFreeDescriptor(&handle_);
        data_.clear();
    }
};
#endif

#ifdef HAVE_KISSFFT
// KissFFT Complex FFT Benchmark
class KissFFTComplexBenchmark : public FFTBenchmark {
private:
    vector<kiss_fft_cpx> in_;
    vector<kiss_fft_cpx> out_;
    kiss_fft_cfg cfg_;
    
public:
    KissFFTComplexBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("Kiss", size, min_duration), cfg_(nullptr) {}
    
    void setup() override {
        in_.resize(size_);
        out_.resize(size_);
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            in_[i].r = sin(t) + 0.5 * sin(5.0 * t);
            in_[i].i = 0.0;
        }
        
        cfg_ = kiss_fft_alloc(size_, 0, nullptr, nullptr);
    }
    
    void execute() override {
        kiss_fft(cfg_, in_.data(), out_.data());
    }
    
    void cleanup() override {
        if (cfg_) kiss_fft_free(cfg_);
        in_.clear();
        out_.clear();
    }
};

// KissFFT Real FFT Benchmark
class KissFFTRealBenchmark : public FFTBenchmark {
private:
    vector<kiss_fft_scalar> in_;
    vector<kiss_fft_cpx> out_;
    kiss_fftr_cfg cfg_;
    
public:
    KissFFTRealBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("Kiss", size, min_duration), cfg_(nullptr) {}
    
    void setup() override {
        in_.resize(size_);
        out_.resize(size_/2 + 1);
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            in_[i] = sin(t) + 0.5 * sin(5.0 * t);
        }
        
        cfg_ = kiss_fftr_alloc(size_, 0, nullptr, nullptr);
    }
    
    void execute() override {
        kiss_fftr(cfg_, in_.data(), out_.data());
    }
    
    void cleanup() override {
        if (cfg_) kiss_fftr_free(cfg_);
        in_.clear();
        out_.clear();
    }
};
#endif

#ifdef HAVE_PFFFT
// PFFFT Complex FFT Benchmark
class PFFFFTComplexBenchmark : public FFTBenchmark {
private:
    vector<float> data_;
    vector<float> work_;
    PFFFT_Setup* setup_;
    
public:
    PFFFFTComplexBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("PFFFT", size, min_duration), setup_(nullptr) {}
    
    void setup() override {
        data_.resize(size_ * 2);
        work_.resize(size_ * 2);
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i * 2] = sin(t) + 0.5 * sin(5.0 * t);
            data_[i * 2 + 1] = 0.0;
        }
        
        setup_ = pffft_new_setup(size_, PFFFT_COMPLEX);
    }
    
    void execute() override {
        pffft_transform_ordered(setup_, data_.data(), data_.data(), work_.data(), PFFFT_FORWARD);
    }
    
    void cleanup() override {
        if (setup_) pffft_destroy_setup(setup_);
        data_.clear();
        work_.clear();
    }
};

// PFFFT Real FFT Benchmark
class PFFFFTRealBenchmark : public FFTBenchmark {
private:
    vector<float> data_;
    vector<float> work_;
    PFFFT_Setup* setup_;
    
public:
    PFFFFTRealBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("PFFFT", size, min_duration), setup_(nullptr) {}
    
    void setup() override {
        data_.resize(size_);
        work_.resize(size_);
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i] = sin(t) + 0.5 * sin(5.0 * t);
        }
        
        setup_ = pffft_new_setup(size_, PFFFT_REAL);
    }
    
    void execute() override {
        pffft_transform_ordered(setup_, data_.data(), data_.data(), work_.data(), PFFFT_FORWARD);
    }
    
    void cleanup() override {
        if (setup_) pffft_destroy_setup(setup_);
        data_.clear();
        work_.clear();
    }
};
#endif

#ifdef HAVE_POCKETFFT
// PocketFFT Complex FFT Benchmark
class PocketFFTComplexBenchmark : public FFTBenchmark {
private:
    vector<complex<double>> data_;
    pocketfft::shape_t shape_;
    pocketfft::stride_t stride_;
    pocketfft::shape_t axes_;
    
public:
    PocketFFTComplexBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("Pocket", size, min_duration) {}
    
    void setup() override {
        data_.resize(size_);
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i] = complex<double>(sin(t) + 0.5 * sin(5.0 * t), 0.0);
        }
        
        shape_ = {static_cast<size_t>(size_)};
        stride_ = {static_cast<ptrdiff_t>(sizeof(complex<double>))};
        axes_ = {0};
    }
    
    void execute() override {
        pocketfft::c2c(shape_, stride_, stride_, axes_, pocketfft::FORWARD, 
                       data_.data(), data_.data(), 1.0);
    }
    
    void cleanup() override {
        data_.clear();
    }
};

// PocketFFT Real FFT Benchmark
class PocketFFTRealBenchmark : public FFTBenchmark {
private:
    vector<double> data_;
    vector<complex<double>> out_;
    pocketfft::shape_t shape_;
    pocketfft::stride_t stride_in_;
    pocketfft::stride_t stride_out_;
    pocketfft::shape_t axes_;
    
public:
    PocketFFTRealBenchmark(int size, double min_duration = 1.0) 
        : FFTBenchmark("Pocket", size, min_duration) {}
    
    void setup() override {
        data_.resize(size_);
        out_.resize(size_/2 + 1);
        
        for (int i = 0; i < size_; ++i) {
            double t = 2.0 * M_PI * i / size_;
            data_[i] = sin(t) + 0.5 * sin(5.0 * t);
        }
        
        shape_ = {static_cast<size_t>(size_)};
        stride_in_ = {static_cast<ptrdiff_t>(sizeof(double))};
        stride_out_ = {static_cast<ptrdiff_t>(sizeof(complex<double>))};
        axes_ = {0};
    }
    
    void execute() override {
        pocketfft::r2c(shape_, stride_in_, stride_out_, axes_, pocketfft::FORWARD,
                       data_.data(), out_.data(), 1.0);
    }
    
    void cleanup() override {
        data_.clear();
        out_.clear();
    }
};
#endif

// Benchmark result holder
struct BenchmarkResult {
    string library;
    double time_us;
    double ratio;  // Relative to baseline (OTFFT)
    
    BenchmarkResult(const string& lib, double time, double base_time) 
        : library(lib), time_us(time) {
        ratio = time_us / base_time;
    }
};

// Benchmark runner and table formatter
class BenchmarkRunner {
private:
    vector<string> enabled_libraries_;
    double min_duration_seconds_;
    static const int LABEL_WIDTH = 15;
    static const int COL_SIZE_WIDTH = 5;
    static const int TIME_WIDTH = 7;
    static const int RATIO_WIDTH = 6;
    
    void detect_enabled_libraries() {
        enabled_libraries_.push_back("OTFFT");
#ifdef HAVE_FFTW3
        enabled_libraries_.push_back("FFTW3");
#endif
#ifdef HAVE_KISSFFT
        enabled_libraries_.push_back("Kiss");
#endif
#ifdef HAVE_PFFFT
        enabled_libraries_.push_back("PFFFT");
#endif
#ifdef HAVE_POCKETFFT
        enabled_libraries_.push_back("Pocket");
#endif
#ifdef HAVE_MKL
        enabled_libraries_.push_back("MKL");
#endif
    }
    
    int calculate_table_width() const {
        int width = LABEL_WIDTH + COL_SIZE_WIDTH + 3;  // Label + Size + separators
        width += TIME_WIDTH + 2;  // OTFFT time + separators
        
        // Each additional library adds: time + ratio + separators
        for (size_t i = 1; i < enabled_libraries_.size(); ++i) {
            width += TIME_WIDTH + RATIO_WIDTH + 4;
        }
        
        return width;
    }
    
    void print_banner() const {
        int width = calculate_table_width();
        cout << string(width, '=') << endl;
        cout << setw(width/2 + 20) << "FFT Performance Comparison Benchmark" << endl;
        cout << string(width, '=') << endl;
        
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
        
        cout << "Comparing:";
        for (const auto& lib : enabled_libraries_) {
            cout << " " << lib;
            if (&lib != &enabled_libraries_.back()) cout << " |";
        }
        cout << endl;
        cout << "Min test duration: " << min_duration_seconds_ << " second(s)" << endl;
        cout << "Times shown in microseconds (μs)" << endl;
        cout << string(width, '=') << endl;
    }
    
    void print_table_header() const {
        cout << left << setw(LABEL_WIDTH) << "Test Type";
        cout << right << setw(COL_SIZE_WIDTH) << "Size" << " |";
        
        // OTFFT (baseline)
        cout << right << setw(TIME_WIDTH) << enabled_libraries_[0] << " |";
        
        // Other libraries with ratio column
        for (size_t i = 1; i < enabled_libraries_.size(); ++i) {
            cout << right << setw(TIME_WIDTH) << enabled_libraries_[i] << " |";
            cout << right << setw(RATIO_WIDTH) << "Ratio" << " |";
        }
        cout << endl;
        
        cout << string(calculate_table_width(), '-') << endl;
    }
    
    void print_results_row(const string& label, int size, const vector<BenchmarkResult>& results) const {
        cout << left << setw(LABEL_WIDTH) << label;
        cout << right << setw(COL_SIZE_WIDTH) << size << " |";
        
        // OTFFT (baseline) time
        cout << fixed << setprecision(2) << setw(TIME_WIDTH) << results[0].time_us << " |";
        
        // Other libraries with ratio
        for (size_t i = 1; i < results.size(); ++i) {
            cout << fixed << setprecision(2) << setw(TIME_WIDTH) << results[i].time_us << " |";
            cout << fixed << setprecision(2) << setw(RATIO_WIDTH-1) << results[i].ratio << "x |";
        }
        cout << endl;
    }
    
public:
    BenchmarkRunner(double min_duration_seconds = 1.0) : min_duration_seconds_(min_duration_seconds) {
        detect_enabled_libraries();
    }
    
    void run_complex_benchmarks(const vector<int>& sizes) {
        print_banner();
        cout << "\n========== COMPLEX FFT TESTS ==========" << endl;
        print_table_header();
        
        for (int size : sizes) {
            vector<BenchmarkResult> results;
            
            // OTFFT (baseline)
            OTFFTComplexBenchmark otfft(size, min_duration_seconds_);
            double baseline_time = otfft.run();
            results.emplace_back("OTFFT", baseline_time, baseline_time);
            
#ifdef HAVE_FFTW3
            FFTW3ComplexBenchmark fftw(size, min_duration_seconds_);
            results.emplace_back("FFTW3", fftw.run(), baseline_time);
#endif

#ifdef HAVE_KISSFFT
            KissFFTComplexBenchmark kiss(size, min_duration_seconds_);
            results.emplace_back("Kiss", kiss.run(), baseline_time);
#endif

#ifdef HAVE_PFFFT
            PFFFFTComplexBenchmark pffft(size, min_duration_seconds_);
            results.emplace_back("PFFFT", pffft.run(), baseline_time);
#endif

#ifdef HAVE_POCKETFFT
            PocketFFTComplexBenchmark pocket(size, min_duration_seconds_);
            results.emplace_back("Pocket", pocket.run(), baseline_time);
#endif

#ifdef HAVE_MKL
            MKLComplexBenchmark mkl(size, min_duration_seconds_);
            results.emplace_back("MKL", mkl.run(), baseline_time);
#endif
            
            print_results_row("Complex FFT", size, results);
        }
    }
    
    void run_real_benchmarks(const vector<int>& sizes) {
        cout << "\n========== REAL FFT TESTS ==========" << endl;
        print_table_header();
        
        for (int size : sizes) {
            vector<BenchmarkResult> results;
            
            // OTFFT (baseline)
            OTFFTRealBenchmark otfft(size, min_duration_seconds_);
            double baseline_time = otfft.run();
            results.emplace_back("OTFFT", baseline_time, baseline_time);
            
#ifdef HAVE_FFTW3
            FFTW3RealBenchmark fftw(size, min_duration_seconds_);
            results.emplace_back("FFTW3", fftw.run(), baseline_time);
#endif

#ifdef HAVE_KISSFFT
            KissFFTRealBenchmark kiss(size, min_duration_seconds_);
            results.emplace_back("Kiss", kiss.run(), baseline_time);
#endif

#ifdef HAVE_PFFFT
            PFFFFTRealBenchmark pffft(size, min_duration_seconds_);
            results.emplace_back("PFFFT", pffft.run(), baseline_time);
#endif

#ifdef HAVE_POCKETFFT
            PocketFFTRealBenchmark pocket(size, min_duration_seconds_);
            results.emplace_back("Pocket", pocket.run(), baseline_time);
#endif

#ifdef HAVE_MKL
            MKLRealBenchmark mkl(size, min_duration_seconds_);
            results.emplace_back("MKL", mkl.run(), baseline_time);
#endif
            
            print_results_row("Real FFT", size, results);
        }
    }
    
    void print_footer() const {
        int width = calculate_table_width();
        cout << "\n" << string(width, '=') << endl;
        cout << "Benchmark completed!" << endl;
        cout << "Note: Ratio shows speedup relative to OTFFT (higher is better for OTFFT)" << endl;
        cout << string(width, '=') << endl;
    }
};

int main() {
    vector<int> sizes = {128, 256, 512, 1024, 2048, 4096, 8192, 16384};
    
    // Run each test for at least 1 second
    BenchmarkRunner runner(1.0);
    runner.run_complex_benchmarks(sizes);
    runner.run_real_benchmarks(sizes);
    runner.print_footer();
    
    return 0;
}

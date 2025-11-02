from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class OTFFTBenchmarkConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def layout(self):
        cmake_layout(self)
    
    def configure(self):
        # FFTW SIMD optimizations
        # Disable long double precision to enable AVX2 SIMD
        self.options["fftw"].precision_longdouble = False
        self.options["fftw"].precision_double = True
        self.options["fftw"].precision_single = True
        # Enable SIMD - try avx2, fallback options if not available: avx, sse2
        try:
            self.options["fftw"].simd = "avx2"
        except:
            try:
                self.options["fftw"].simd = "avx"
            except:
                pass  # Use default
    
    def requirements(self):
        # Always required FFT libraries
        self.requires("otfft/11.5@")
        # FFTW with SIMD optimizations enabled
        self.requires("fftw/3.3.10")
        self.requires("kissfft/131.1.0")
        self.requires("pffft/cci.20210511")
        self.requires("pocketfft/0.0.0.cci.20240801")
        
        # Intel MKL via Conan is x86_64 only
        # Note: This is for Conan packages. System MKL detection in CMake
        # will still work on x86_64 if MKL is installed locally
        # if self.settings.arch == "x86_64":
        #     self.requires("intel-oneapi-mkl/2023.2.0")  # Not available in Conan Center
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

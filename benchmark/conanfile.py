from conans import ConanFile, CMake

class OTFFTBenchmarkConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    options = {"with_mkl": [True, False]}
    default_options = {"with_mkl": True}
    generators = "cmake_find_package", "cmake_paths"
    
    def configure(self):
        # FFTW SIMD optimizations
        # Disable long double precision to enable SIMD optimizations
        self.options["fftw"].precision_longdouble = False
        self.options["fftw"].precision_double = True
        self.options["fftw"].precision_single = True
        
        # Enable SIMD based on architecture
        if self.settings.arch in ["x86", "x86_64"]:
            # Enable SIMD for x86 - try avx2, fallback options if not available: avx, sse2
            try:
                self.options["fftw"].simd = "avx2"
            except:
                try:
                    self.options["fftw"].simd = "avx"
                except:
                    try:
                        self.options["fftw"].simd = "sse2"
                    except:
                        pass  # Use default
        else:
            # For ARM and other architectures, disable explicit SIMD setting
            # FFTW will auto-detect NEON on ARM
            self.options["fftw"].simd = False
    
    def requirements(self):
        # FFTW as the primary/standard FFT library - always required
        self.requires("fftw/3.3.10")
        
        # OTFFT - required on x86_64, will be dynamically detected by CMake
        if self.settings.arch == "x86_64":
            self.requires("otfft/11.5@")
        
        # Other FFT libraries for comparison
        self.requires("kissfft/131.1.0")
        self.requires("pffft/cci.20210511")
        self.requires("pocketfft/0.0.0.cci.20240801")
        
        # Intel MKL via Conan is x86_64 only
        # Note: This is for Conan packages. System MKL detection in CMake
        # will still work on x86_64 if MKL is installed locally
        # if self.settings.arch == "x86_64" and self.options.with_mkl:
        #     self.requires("intel-oneapi-mkl/2023.2.0")  # Not available in Conan Center
    
    def build(self):
        cmake = CMake(self)
        # Pass option to disable MKL detection in CMake
        if not self.options.with_mkl:
            cmake.definitions["DISABLE_MKL"] = "ON"
        cmake.configure()
        cmake.build()

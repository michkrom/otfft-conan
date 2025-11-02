from conans import ConanFile, CMake, tools
import os

class OtfftConan(ConanFile):
    name = "otfft"
    version = "11.5"

    # Optional metadata
    license = "MIT"
    author = "Takuya OKAHISA"
    url = "https://github.com/DEWETRON/OTFFT"
    description = "A high-speed FFT library using Stockham's algorithm and AVX/SSE"
    topics = ("fft", "performance", "avx", "sse", "simd", "header-only")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    no_copy_source = True
    
    def export_sources(self):
        # Export sources if they exist in the current directory
        self.copy("*.h")
        self.copy("*.hpp")
        self.copy("src/*")
        self.copy("include/*")
        self.copy("inc/*")
        self.copy("CMakeLists.txt")
        self.copy("cmake/*")
        self.copy("LICENSE*")
        self.copy("README*")

    def source(self):
        # If sources are not exported, try to get them from GitHub
        if not os.path.exists(os.path.join(self.source_folder, "CMakeLists.txt")):
            # Try different possible URLs for OTFFT
            try:
                tools.get("https://github.com/DEWETRON/OTFFT/archive/refs/heads/master.zip", 
                         destination=self.source_folder, strip_root=True)
            except:
                try:
                    tools.get("https://github.com/DEWETRON/otfft/archive/refs/heads/master.zip", 
                             destination=self.source_folder, strip_root=True)
                except:
                    # If download fails, warn
                    self.output.warn("Could not download OTFFT sources. Please place OTFFT source manually.")

    def build(self):
        cmake = CMake(self)
        # OTFFT has options for different SIMD levels
        cmake.definitions["OTFFT_BUILD_ONLY_HEADERS"] = "ON"
        cmake.configure()
        cmake.build()

    def package_id(self):
        self.info.header_only()

    def package(self):
        # Copy license
        self.copy("LICENSE*", dst="licenses", src=self.source_folder, keep_path=False)
        
        # Copy headers from inc directory (where OTFFT keeps them)
        self.copy("*.h", dst="include", src=os.path.join(self.source_folder, "inc"), keep_path=False)
        
        # Copy generated config header from build folder
        self.copy("otfft_config.h", dst="include", src=os.path.join(self.build_folder, "src"), keep_path=False)
        
        # Copy library files
        self.copy("*.a", dst="lib", src=self.build_folder, keep_path=False)
        self.copy("*.lib", dst="lib", src=self.build_folder, keep_path=False)
        self.copy("*.so*", dst="lib", src=self.build_folder, keep_path=False)
        self.copy("*.dylib", dst="lib", src=self.build_folder, keep_path=False)
        self.copy("*.dll", dst="bin", src=self.build_folder, keep_path=False)

    def package_id(self):
        # OTFFT is NOT header-only anymore since it builds a library
        pass

    def package_info(self):
        # Link the OTFFT library
        self.cpp_info.libs = ["otfft"]
        
        # Add compiler flags for optimal performance
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            self.cpp_info.cppflags.append("-mavx")
            self.cpp_info.cppflags.append("-msse2")
            self.cpp_info.cppflags.append("-fopenmp")
        elif self.settings.compiler == "Visual Studio":
            self.cpp_info.cppflags.append("/arch:AVX")
            self.cpp_info.cppflags.append("/openmp")

        # Set C++ standard requirement
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            self.cpp_info.cppflags.append("-std=c++11")



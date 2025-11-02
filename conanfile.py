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
    topics = ("fft", "performance", "avx", "sse", "simd")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"with_openmp": [True, False]}
    default_options = {"with_openmp": False}
    generators = "cmake_find_package", "cmake_paths"
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
            try:
                tools.get("https://github.com/DEWETRON/otfft/archive/refs/heads/master.zip", 
                            destination=self.source_folder, strip_root=True)
            except:
                # If download fails, warn
                self.output.warn("Could not download OTFFT sources. Please place OTFFT source manually.")

    def build(self):
        # Only patch CMakeLists.txt to disable OpenMP if with_openmp is False
        try:
            if not self.options.with_openmp:
                tools.replace_in_file(
                    os.path.join(self.source_folder, "CMakeLists.txt"),
                    "find_package(OpenMP REQUIRED)",
                    "# find_package(OpenMP) - disabled by conan",
                    strict=False
                )
            else:
                tools.replace_in_file(
                    os.path.join(self.source_folder, "CMakeLists.txt"),
                    "find_package(OpenMP)",
                    "find_package(OpenMP REQUIRED)",
                    strict=False
                )
        except:
            # Pattern not found or already patched, continue anyway
            self.output.info("OpenMP patching skipped (pattern not found)")
        
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
        
        # Add OpenMP flags if enabled
        if self.options.with_openmp:
            if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
                self.cpp_info.cppflags.append("-fopenmp")
                self.cpp_info.sharedlinkflags.append("-fopenmp")
                self.cpp_info.exelinkflags.append("-fopenmp")
            elif self.settings.compiler == "Visual Studio":
                self.cpp_info.cppflags.append("/openmp")

        # Set C++ standard requirement
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            self.cpp_info.cppflags.append("-std=c++11")



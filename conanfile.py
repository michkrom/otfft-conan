from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import get, copy
from conan.errors import ConanException
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
    generators = "CMakeDeps", "CMakeToolchain"
    no_copy_source = True
    
    def layout(self):
        cmake_layout(self)
    
    def export_sources(self):
        # Export sources if they exist in the current directory
        copy(self, "*.h", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "*.hpp", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "src/*", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "include/*", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "inc/*", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "CMakeLists.txt", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "cmake/*", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "LICENSE*", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "README*", src=self.recipe_folder, dst=self.export_sources_folder)

    def source(self):
        # If sources are not exported, try to get them from GitHub
        if not os.path.exists(os.path.join(self.source_folder, "CMakeLists.txt")):
            # Try different possible URLs for OTFFT
            try:
                get(self, "https://github.com/DEWETRON/otfft/archive/refs/heads/master.zip", 
                    destination=self.source_folder, strip_root=True)
                self.output.info("Successfully downloaded OTFFT sources from DEWETRON/otfft")
            except Exception as e2:
                # If download fails, raise an error
                raise ConanException(f"Could not download OTFFT sources. Tried both URLs. Errors: {e}, {e2}")

    def build(self):
        cmake = CMake(self)
        # Configure and build OTFFT
        cmake.configure()
        cmake.build()

    def package_id(self):
        # OTFFT builds different libraries for different architectures and SIMD levels
        # It's not header-only, so we need full package_id
        pass

    def package(self):
        # Copy license
        copy(self, "LICENSE*", dst=os.path.join(self.package_folder, "licenses"), 
             src=self.source_folder, keep_path=False)
        
        # Copy headers from inc directory (where OTFFT keeps them)
        copy(self, "*.h", dst=os.path.join(self.package_folder, "include"), 
             src=os.path.join(self.source_folder, "inc"), keep_path=False)
        
        # Copy generated config header from build folder
        copy(self, "otfft_config.h", dst=os.path.join(self.package_folder, "include"), 
             src=os.path.join(self.build_folder, "src"), keep_path=False)
        
        # Copy library files
        copy(self, "*.a", dst=os.path.join(self.package_folder, "lib"), 
             src=self.build_folder, keep_path=False)
        copy(self, "*.lib", dst=os.path.join(self.package_folder, "lib"), 
             src=self.build_folder, keep_path=False)
        copy(self, "*.so*", dst=os.path.join(self.package_folder, "lib"), 
             src=self.build_folder, keep_path=False)
        copy(self, "*.dylib", dst=os.path.join(self.package_folder, "lib"), 
             src=self.build_folder, keep_path=False)
        copy(self, "*.dll", dst=os.path.join(self.package_folder, "bin"), 
             src=self.build_folder, keep_path=False)

    def package_info(self):
        # OTFFT builds multiple libraries for different SIMD levels
        # Link all available libraries
        self.cpp_info.libs = ["otfft_common"]
        
        # Add SIMD-specific libraries based on architecture
        if self.settings.arch in ["x86_64", "x86"]:
            self.cpp_info.libs.extend(["otfft_sse2", "otfft_avx", "otfft_avx2"])
        else:
            # For ARM and other architectures, only common library
            pass
        
        # Add compiler flags for optimal performance
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            self.cpp_info.cppflags.append("-msse2")
            if self.settings.arch in ["x86_64", "x86"]:
                self.cpp_info.cppflags.append("-mavx")
            self.cpp_info.cppflags.append("-fopenmp")
        elif self.settings.compiler == "Visual Studio":
            self.cpp_info.cppflags.append("/arch:AVX")
            self.cpp_info.cppflags.append("/openmp")

        # Set C++ standard requirement
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            self.cpp_info.cppflags.append("-std=c++11")



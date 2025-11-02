from conans import ConanFile, CMake, tools
import os


class OtfftTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="bin", keep_path=False)
        self.copy('*.so*', dst='bin', keep_path=False)

    def test(self):
        if not tools.cross_building(self):
            # For header-only library, we just need to check if compilation works
            self.output.info("OTFFT headers should be available for inclusion")
            bin_path = os.path.join("bin", "test")
            self.run(bin_path)
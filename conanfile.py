from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import copy
from conan.tools.scm import Version

class StduuidConan(ConanFile):
    name = "stduuid"
    description = "A C++17 cross-platform implementation for UUIDs"
    topics = ("conan", "uuid", "guid")
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/mariusbancila/stduuid"
    license = "MIT"
    settings = "os", "compiler"
    version = "1.0.0"
    
    no_copy_source = True
    _source_subfolder = ""
    
    def export_sources(self):
        copy(self, "LICENSE", self.recipe_folder, self.export_sources_folder)
        copy(self, "include/*", self.recipe_folder, self.export_sources_folder)

    def requirements(self):
        self.requires("ms-gsl/2.0.0")
        if self.settings.os != "Windows":
            self.requires("libuuid/1.0.3")

    def configure(self):
        version = Version( self.settings.compiler.version )
        compiler = self.settings.compiler
        if self.settings.compiler.cppstd and \
           not any([str(self.settings.compiler.cppstd) == std for std in ["17", "20", "23", "gnu17", "gnu20"]]):
            raise ConanInvalidConfiguration(f"stduuid requires at least c++17, current is {self.settings.compiler.cppstd}")
        elif compiler == "msvc"and version < "15":
            raise ConanInvalidConfiguration("stduuid requires at least Visual Studio version 15")
        else:
            if ( compiler == "gcc" and version < "7" ) or ( compiler == "clang" and version < "5" ):
                raise ConanInvalidConfiguration("stduuid requires a compiler that supports at least C++17")
            elif compiler == "apple-clang"and version < "10":
                raise ConanInvalidConfiguration("stduuid requires a compiler that supports at least C++17")

    def package(self):
        copy(self, "*.h", self.source_folder, self.package_folder)

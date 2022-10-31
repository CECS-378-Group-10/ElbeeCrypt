## Imports
from conans import ConanFile, Meson
import platform, subprocess


## Conan builder class
## Based on Conan's Meson plugin: https://docs.conan.io/en/latest/reference/build_helpers/meson.html
class Builder(ConanFile):
	## Project settings
	#Name, version, and dependencies (name and version are to be determined by Meson, not Conan)
	requires = ["libsodium/cci.20220430"]
	
	#Compiler settings and source resolution
	settings = "os", "compiler", "build_type", "arch"
	generators = "pkg_config"
	exports_sources = "src/*"


	## Define the build stage
	def build(self):
		meson = Meson(self)
		meson.configure(build_folder=self.source_folder + "/build") #Meson should build in a folder called "build" in the source directory
		meson.build()
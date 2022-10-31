## Imports
from conans import ConanFile, Meson
import platform, subprocess

## Utility functions
def which(cmdName: str) -> list[str]:
	#Choose the correct locator command
	isWindows = platform.system().lower() == "windows"
	cmd = "where" if isWindows else "which"
	
	#Execute the command, split the output, and return
	output = subprocess.run([cmd, cmdName], capture_output=True).stdout.decode("utf-8")
	return output.strip().split("\r\n" if isWindows else "\n")
	
	
## Conan builder class
## Based on Conan's Meson plugin: https://docs.conan.io/en/latest/reference/build_helpers/meson.html
class Builder(ConanFile):
	## Project settings
	#Name, version, and dependencies (name and version are to be determined by Meson, not Conan)
	requires = ["poco/1.12.2", "libsodium/cci.20220430"]
	
	#Compiler settings and source resolution
	settings = "os", "compiler", "build_type", "arch"
	generators = "pkg_config"
	exports_sources = "src/*"


	## Define the build stage
	def build(self):
		meson = Meson(self)
		meson.configure(build_folder=self.source_folder + "/build") #Meson should build in a folder called "build" in the source directory
		meson.build()
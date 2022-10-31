## Compiler Utilities ##
from enum import Enum

## Imports
from dependencies.os_util import *
from dependencies.vslocate import get_vs_installations
from dependencies.which import which
import os, platform, re, subprocess, sys

## Enums
class CompilerNames(Enum):
	""" 
	Compiler name definitions. Names are based on those that Conan 
	accepts which can be found here: 
	https://docs.conan.io/en/latest/extending/custom_settings.html 
	"""
	GCC = "gcc",
	MSVC = "Visual Studio",
	UNKNOWN = "UNKNOWN"

class CppStd(Enum):
	"""
	C++ standards. Based on Conan's accepted list of C++ standards
	which can be found here: https://docs.conan.io/en/latest/extending/custom_settings.html
	"""
	Cpp98 = "98",
	Cpp03 = "03",
	Cpp11 = "11",
	Cpp17 = "17",
	Cpp20 = "20",
	Cpp23 = "23",
	CppNone = "None"


## Functions
def getAvailableCompilers() -> list[CompilerNames]:
	"""
	Gets a list of the system's available compilers.

	Returns:
		list[CompilerNames]: The system's available compilers
	"""

	#Create the list of compilers supported
	compilers: list[CompilerNames] = []

	#Check for GCC
	if len(which("gcc")) > 0 and len(which("g++")) > 0: compilers.append(CompilerNames.GCC)

	#Check if the target is Windows
	if getHostOS() == OSNames.Windows:
		#Check for MSVC
		if len(get_vs_installations()) > 0: compilers.append(CompilerNames.MSVC)

	#Return the list of compilers
	return compilers

def getGCCVersion() -> str:
	"""
	Gets the version of the first available GCC toolchain.

	Returns:
		str: The version of the found GCC toolchain
	"""

	#Check if the host has GCC installed, and return 0 if this is not the case
	if len(which("gcc")) < 1:
		return "0"

	#Execute the command and split the output
	output = subprocess.run(["gcc", "--version"], capture_output=True).stdout.decode("utf-8")
	gccInfoLine = output.strip().split("\r\n" if getHostOS() == OSNames.Windows else "\n")[0]

	#Isolate just the version string and return it
	return re.search(r"([0-9]+\.)+[0-9]+", gccInfoLine).group(0)

def getMSVCVersion() -> str:
	"""
	Gets the version of the first available MSVC toolchain.

	Returns:
		str: The version of the found MSVC toolchain
	"""

	#Get the MSVC information list
	msvcList: list[tuple(str, str)] = get_vs_installations()

	#Check if the host has MSVC installed, and return 0 if this is not the case
	if len(msvcList) < 1:
		return "0"

	#Isolate just the version string and return it
	return msvcList[0][0]
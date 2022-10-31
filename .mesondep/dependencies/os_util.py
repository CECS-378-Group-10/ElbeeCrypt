## OS Utilities; based on https://stackoverflow.com/a/54837707 ##


## Imports
from enum import Enum
import os, platform, sys

## Enums
class ArchNames(Enum):
	""" 
	Architecture definitions. Names are based on those that Conan 
	accepts which can be found here: 
	https://docs.conan.io/en/latest/extending/custom_settings.html 
	"""
	x86_64 = "x86_64",
	x86 = "x86",
	arm64 = "armv8",
	arm32 = "armv7",
	UNKNOWN = "UNKNOWN"

class OSNames(Enum):
	"""
	OS name definitions. Names are based on those that Conan 
	accepts which can be found here: 
	https://docs.conan.io/en/latest/extending/custom_settings.html 
	"""
	Windows = "Windows",
	MacOS = "Macos",
	Linux = "Linux",
	FreeBSD = "FreeBSD",
	SunOS = "SunOS",
	UNKNOWN = "UNKNOWN"


## Functions
def getHostArch() -> ArchNames:
	"""
	Gets the host's architecture as a standard name.

	Returns:
		ArchNames: The normalized architecture as an enum object
	"""

	#Get the OS architecture via platform.machine()
	rawArch = platform.machine().lower()

	#Check all four types of common arches
	#x86_64
	if (rawArch == "x86_64" or rawArch == "amd64") and platform.architecture()[0] != "32bit":
		return ArchNames.x86_64
	#x86
	if rawArch == "i386" or rawArch == "i686" or rawArch == "i86pc":
		return ArchNames.x86
	#arm64
	if (rawArch == "arm64" or rawArch == "aarch64" or rawArch.contains("armv8")) and platform.architecture()[0] != "32bit":
		return ArchNames.arm64
	#arm32
	if rawArch.contains("armv6") or rawArch.contains("armv7"):
		return ArchNames.arm32

	#Default return (shouldn't happen)
	return ArchNames.UNKNOWN

def getHostOS() -> OSNames:
	"""
	Gets the host's OS as a standard name.

	Returns:
		OSNames: The normalized OS name as an enum object
	"""

	#Get the OS name via both os.name and sys.platform
	osName = os.name.lower()
	platformName = sys.platform.lower()

	#Handle Windows OSes (Windows can masquerade as POSIX via MinGW or Cygwin, so handle it first)
	if osName == "nt" or platformName == "win32" or platformName == "msys" or platformName == "cygwin":
		return OSNames.Windows

	#Handle the POSIX OSes (Linux, Unix, MacOS)
	if osName == "posix":
		#MacOS
		if platformName == "darwin":
			return OSNames.MacOS
		#Linux
		if platformName == "linux":
			return OSNames.Linux
		#FreeBSD
		if platformName.contains("freebsd"):
			return OSNames.FreeBSD
		#Solaris/SunOS
		if platformName.contains("sun") or platformName.contains("solaris"):
			return OSNames.SunOS

	#Default return (shouldn't happen)
	return OSNames.UNKNOWN
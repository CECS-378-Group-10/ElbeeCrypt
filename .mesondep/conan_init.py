### Conan Initializer for Meson & Conan Projects ###
## Adapted from: https://lightrun.com/answers/conan-io-conan-mingwgcc-
## Inspired by: https://github.com/jenglamlow/meson-conan-cpp
## Call via: conan-init.py (interactive menu)
## Example: python .mesondep/conan-init.py


## Imports
from configparser import ConfigParser
from dataclasses import dataclass
from dependencies.compiler_util import *
from dependencies.enum_util import *
from dependencies.mutable_enum import MutableEnum
from dependencies.os_util import *
from pathlib import Path
from string import Template
import os, shutil

## Constants
CONAN_PROFILE_DIR = "./files/conan-profiles/"
""" Support directory for the included Conan profiles. """

USR_CONAN_PROFILE_DIR = ".conan/profiles/"
""" The directory where Conan stores profiles in the user folder. """

OUTPUT_PROFILE_NAME = "conan-generated"
""" The name for the generated Conan profile. """

DEFAULT_CONAN_PROFILE_NAME = "default"
""" The name of the default Conan profile located in `~/.conan/profiles/`. """


## Classes
@dataclass
class ConanSettings:
	""" Simple data class for Conan settings. """
	osName: OSNames
	usrArch: MutableEnum #ArchNames
	isWindows: bool
	availableCompilers: list[CompilerNames]
	usrCompiler: MutableEnum #CompilerNames
	cppStd: MutableEnum #CppStd

	def __init__(self) -> None:
		pass


## Functions
def copyConanProfile(profilePath: Path, conanUserPath: Path, settings: ConanSettings):
	"""
	Copies a given Conan config to the user's Conan profiles
	folder. If the profile to copy contains unfilled placeholders,
	those will be filled. If the path is `.`, the default profile
	that already exists will simply be duplicated and renamed.

	Args:
		profilePath (Path): The profile file to copy
		conanUserPath (Path): The users's Conan profiles folder
		settings (ConanSettings): The relevant Conan settings that were previously selected

	Raises:
		ValueError: If one of the paths is invalid or the file type is incorrect
	"""

	#Set the output path
	outputPath: Path = conanUserPath / OUTPUT_PROFILE_NAME

	#Check if the path is blank (if so, simply copy the default profile)
	if str(profilePath) == ".":
		#Duplicate the default Conan profile and exit
		shutil.copy2(conanUserPath / DEFAULT_CONAN_PROFILE_NAME, outputPath)
		return

	#Perform argument parsing
	if not profilePath.exists() or not conanUserPath.exists():
		raise ValueError("One or more paths are invalid. Check the command arguments and try again.")
	if not profilePath.is_file():
		raise ValueError("The profile path must point to a file.")
	if not conanUserPath.is_dir():
		raise ValueError("The user path must point to a directory.")

	#Read in the selected profile as an ini file
	profileIni: ConfigParser = ConfigParser()
	profileIni.read(str(profilePath), "utf-8")

	#Get the replaceable placeholder attributes that aren't already in the settings
	compilerVersion: str = (getGCCVersion() if settings.usrCompiler == CompilerNames.GCC else getMSVCVersion()).split(".")[0]
	
	#Do placeholder replacement on the ini object
	profileIni["settings"]["compiler.version"] = Template(profileIni["settings"]["compiler.version"]).substitute({"compiler_version": compilerVersion})
	profileIni["settings"]["compiler.cppstd"] = Template(profileIni["settings"]["compiler.cppstd"]).substitute({"compiler_cppstd": settings.cppStd.getData().value[0]})

	#Add in the GCC paths
	if settings.usrCompiler == CompilerNames.GCC:
		profileIni["options"]["build_requires"]["env"]["CC"] = Template(profileIni["options"]["build_requires"]["env"]["CC"]).substitute({"compiler_cc_path": which("gcc")[0]})
		profileIni["options"]["build_requires"]["env"]["CXX"] = Template(profileIni["options"]["build_requires"]["env"]["CXX"]).substitute({"compiler_cxx_path": which("g++")[0]})

	#Check if the profile already exists
	if outputPath.exists():
		#Delete the existing profile
		os.remove(str(outputPath))

	#Write the profile to the user's Conan folder
	with open(str(outputPath), "w") as ini:
		profileIni.write(ini)

def makeSelection(items: list[Enum], selection: MutableEnum):
	"""
	Allows the user to select an option from a given list.

	Args:
		items (list[Enum]): The list of options to choose from
		selection (MutableEnum): The option that the user will change (mutable enum)
	"""

	#Get the list of items
	itemsStr = ""
	for i in range(0, len(items)):
		itemsStr += "({}) {}".format(i, items[i].name)
		if i < len(items) - 1: itemsStr += ", "

	#Loop while the correct config item hasn't been selected
	selected = False
	while not selected:
		#Prompt the user for input
		print("Select the desired option: {}".format(itemsStr))
		select = input("> ")

		#If the input is a number in range, make that the selection and break
		if select.isnumeric() and (int(select) >= 0 and int(select) <= len(items) - 1):
			selection.setData(items[int(select)])
			selected = True
		else:
			#Re-prompt the user
			print("\"{}\" is not a valid selection. The selection must be a valid number in range [{}, {}]".format(select, 0, len(items) - 1))

def printSettings(settings: ConanSettings):
	"""
	Prints the settings menu for the user.

	Args:
		settings (ConanSettings): The Conan settings object to display
	"""

	#Print settings to the user
	print("\tOS: {}".format(settings.osName.name))
	print("\t(0) Arch: {}".format(settings.usrArch.getData().name))
	print("\tAvailable compilers: {}".format(enumNamesToString(settings.availableCompilers)))
	print("\t(1) Selected compiler: {}".format(settings.usrCompiler.getData().name))
	print("\t(2) C++ standard: {}".format(settings.cppStd.getData().name))

	#If the compiler is GCC and the OS is Windows, issue a warning
	if settings.osName is OSNames.Windows and settings.usrCompiler.getData() is CompilerNames.GCC:
		print("/!\\ WARNING: Compiling with GCC on Windows may cause compilation issues with dependencies. Use with caution.")

def selectBestConanProfile(settings: ConanSettings) -> Path:
	"""
	Selects the best Conan profile based on the settings
	given. If no suitable alternative is found, the default
	config in ~/.conan/profiles will be used instead.

	Args:
		settings (ConanSettings): _description_

	Returns:
		Path: The path to the selected Conan profile. Outputs "." if none was found
	"""

	#Set the output path
	out: Path = Path()

	#Get the Conan profile directory as an absolute path
	conanProfileDir: Path = Path(sys.path[0] + "/" + CONAN_PROFILE_DIR + "/").absolute()

	#Loop over the available Conan profiles
	for profile in os.listdir(str(conanProfileDir)):
		#Derive an absolute path to the current profile
		profilePath: Path = (conanProfileDir / profile).absolute()

		#Read in the current profile as an ini file
		profileIni: ConfigParser = ConfigParser()
		profileIni.read(str(profilePath), "utf-8")

		#Get the OS, architecture, and compiler of the current profile
		profileOS: str = profileIni["settings"]["os"]
		profileArch: str = profileIni["settings"]["arch"]
		profileCompiler: str = profileIni["settings"]["compiler"]

		#If the current profile settings and the user settings match, set this profile as the one to return
		osNamesEq = settings.osName.value[0] == profileOS 
		archNamesEq = settings.usrArch.getData().value[0] == profileArch
		compilerNamesEq = settings.usrCompiler.getData().value[0] == profileCompiler
		if osNamesEq and archNamesEq and compilerNamesEq:
			out = profilePath
	
	#Return the path
	return out


## Main
if __name__ == '__main__':
	#Get the host OS and architecture
	hostOS = getHostOS()
	hostArch = getHostArch()

	#Create the settings object and initialize it
	settings: ConanSettings = ConanSettings()
	settings.osName = hostOS
	settings.usrArch = MutableEnum(hostArch)
	settings.availableCompilers = getAvailableCompilers()
	settings.cppStd = MutableEnum(CppStd.Cpp17) #C++17 by default (a good mix of new features and compatibility)

	#Create tracking variables
	settings.isWindows = hostOS == OSNames.Windows 

	#Set the compiler
	settings.usrCompiler = MutableEnum(settings.availableCompilers[len(settings.availableCompilers) - 1]) #Will be MSVC if the target is Windows and its available

	#Have the user change settings if requested
	print("OS and compiler settings auto-detected. ", end = '')
	doneConfiging = False
	while not doneConfiging:
		print("The following settings are in force for Conan:")
		printSettings(settings)
		print("Enter the numbers you wish to change or press enter to finalize the selected configuration.")
		selection = input("> ")

		#Check if the user input is anything but a number
		if len(selection) > 0:
			#Check if the selection is non-numeric
			if not selection.isnumeric():
				print("Invalid selection. The selection must be a non-negative number.\n")
				continue

			#Prompt for a section (piece one)
			print("Changing option {} - ".format(selection), end = '')

			#Set the allowed arches via heuristics on the derived architecture
			allowedArches: list[ArchNames] = []
			if hostOS is not OSNames.MacOS: #MacOS does not support 32-bit arm or x86
				allowedArches.append(ArchNames.x86 if ("x86" in str(hostArch.name)) else ArchNames.arm32)
			if "64" in str(hostArch.name):
				allowedArches.append(ArchNames.x86_64 if ("x86" in str(hostArch.name)) else ArchNames.arm64)

			#Switch over the current character
			match int(selection):
				case 0: #Change the architecture
					print("architecture...")
					makeSelection(allowedArches, settings.usrArch)

				case 1: 
					#Change the compiler
					print("compiler...")
					makeSelection(settings.availableCompilers, settings.usrCompiler)

				case 2:
					#Change the C++ standard
					print("C++ standard...")
					makeSelection([member for member in CppStd], settings.cppStd)
								
				case _: 
					print("<Invalid option; skipping>\n")

			#Add a line break
			print("")
		else:
			#Finish up the configuration step
			doneConfiging = True

	#Reprint the settings
	print("\nConfiguring Conan with the following settings:")
	printSettings(settings)
	print("")

	#Select the best Conan profile based on the user's settings
	selectedPath: Path = selectBestConanProfile(settings)
	if str(selectedPath) == ".":
		print("/!\\ WARNING: An appropriate Conan profile was unable to be selected. The default one generated by Conan during the `conan config init` step will be used instead.")

	#Copy the Conan config to the user's profile folder
	print("The following Conan profile has been selected based on the given settings: {}".format(selectedPath.name if str(selectedPath) != "." else "<default>"))
	print("If the above profile isn't correct, simply rerun this script again.")
	copyConanProfile(selectedPath, Path.home() / USR_CONAN_PROFILE_DIR, settings)
	print("Profile copied. It can be found under the following path: {}".format(Path.home() / USR_CONAN_PROFILE_DIR / OUTPUT_PROFILE_NAME))
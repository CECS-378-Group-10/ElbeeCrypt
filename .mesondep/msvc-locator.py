### MSVC Locator for Meson ###
## Adapted from: https://github.com/mjansson/vslocate
## Call via: msvc-locator.py
## Example: python .mesondep/msvc-locator.py

## Imports
from pathlib import Path
from vslocate import get_vs_installations

## Functions
def getVCVarsAllLocations() -> list[Path]:
	"""
	Derives a list of all of the `vcvarsall.bat` locations on the
	system. This function depends on the `get_vs_installations()`
	function from `vslocate.py`.

	Returns:
		list[Path]: A list of the found paths. Will be empty if no results were found
	"""

	#Create a list to store all of the vcvarsall.bat locations
	vcvarsallPaths: list[Path] = []

	#Get all of the available VS installations
	vsInstalls: list[(str, str)] = get_vs_installations()

	#Loop over each install
	for pair in vsInstalls:
		#Get the current path (we don't need the version)
		basePath = Path(pair[1])

		#Attempt to derive the location of vcvarsall.bat
		vcvarsall = Path.joinpath(basePath, "VC", "Auxiliary", "Build", "vcvarsall.bat")

		#Add the path to the output list if it exists
		if vcvarsall.exists() and vcvarsall.is_file(): vcvarsallPaths.append(vcvarsall)
	
	#Return the list of vcvarsall locations
	return vcvarsallPaths

#Main
if __name__ == "__main__":
	for vcvarsLoc in getVCVarsAllLocations():
		print(str(vcvarsLoc))
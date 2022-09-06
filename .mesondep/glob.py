### File Globber for Meson ###
## Adapted from: https://stackoverflow.com/a/2186565
## Call via: glob.py "directory" "glob list (delimited by spaces)"
## Example: python .mesondep/glob.py "include/" "*.h *.hpp *.hxx"


## Imports
from pathlib import Path
import os, sys

## Functions
def glob(directory: str, extensions: list[str]) -> list[Path]:
	"""
	Globs a directory for files with specific extensions, building
	a list of the paths to the globbed files.

	:param directory: The topmost directory to glob
	:param extensions: The list of extensions to add to the glob list
	:return: A list of the globbed files as absolute file paths
	"""

	#Get the current directory
	cdir = os.getcwd()

	#Create an array to store the globbed paths
	globbedPaths: list[Path] = []

	#Loop over the types array
	for extension in extensions:
		#Glob the necessary files (input is from args)
		for path in Path(cdir).joinpath(directory).rglob(extension):
			globbedPaths.append(path)

	#Return the globbed paths array
	return globbedPaths

## Main
if __name__ == "__main__":
	#Get the globbed files
	globbed: list[Path] = glob(sys.argv[1], sys.argv[2].split(' '))

	#Print each globbed path if there is at least one element in the array
	if len(globbed) > 0:
		for globbedPath in globbed: print(globbedPath)
	else: print("<EMPTY>")
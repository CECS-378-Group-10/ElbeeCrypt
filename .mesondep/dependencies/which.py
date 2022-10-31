## Imports
import os, platform, subprocess, sys

## Utility functions
def which(cmdName: str) -> list[str]:
	"""
	Portable proxy for the `which`/`where` command.

	Args:
		cmdName (str): The binary name to look for

	Returns:
		list[str]: The list of matches for the given binary name
	"""

	#Get the OS name via both os.name and sys.platform
	osName = os.name.lower()
	platformName = sys.platform.lower()

	#Choose the correct locator command
	isWindows = osName == "nt" or platformName == "win32" or platformName == "msys" or platformName == "cygwin"
	cmd = "where" if isWindows else "which"
	
	#Execute the command and split the output
	output = subprocess.run([cmd, cmdName], capture_output=True).stdout.decode("utf-8")
	split = output.strip().split("\r\n" if isWindows else "\n")

	#If the split output has zero entries, return a zero-length list
	if len(split[0]) > 0: return split
	else: return []


## Main
if __name__ == '__main__':
	#Perform argument parsing
	if len(sys.argv) != 2:
		print("Usage: {} <command name>".format(sys.argv[0]))
		exit(-1)
		
	#Run which
	whiches = which(sys.argv[1])
	if len(whiches) > 0:
		for which in whiches:
			print(which)
	else:
		print("<Item '{}' not found on path>".format(sys.argv[1]))
### Resource Processor for Meson ###
## Adapted from: https://gist.github.com/rakete/c6f7795e30d5daf43bb600abc06a9ef4
## Call via: xxd.py "directory"
## Example: python .mesondep/xxd.py "resources/"


## Imports
from pathlib import Path
import os, sys

## Functions
def xxd(filePath: str) -> str:
	"""
	Converts a file to a byte array sequence for use in embedding the 
	said file as a resource in C/C++. Equivalent to running `xxd -i` 
	on the file in question, but in a cross-platform way.

	:param filePath: The file to convert
	:return: A string containing the resultant byte array and length
	"""

	#Ensure the file path exists before continuing
	if not os.path.exists(filePath):
		raise Exception("The file at path '{}' doesn't exist or no path was given.", filePath)

	#Create a variable to store the output
	output: str = ""

	#Attempt to open the file at the path
	with open(filePath, 'r') as file:
		#Get the name of the file
		array_name = filePath.replace('\\', '_').replace('/', '_').replace('.', '_')

		#Initialize the variables in use
		output: str = "unsigned char %s[] = {" % array_name
		length: int = 0

		#Loop infinitely (usually dangerous, but breaks at the end of the file)
		while True:
			#Read in a 12 byte chunk from the file
			buf = file.read(12)

			#If the buffer is empty, break out of the loop, otherwise append a line break
			if not buf:
				output = output[:-2]
				break
			else:
				output += "\n\t"

			#Loop over the buffer and add each section to the output as well as increase the length by 1
			for i in buf:
				output += "0x%02x, " % ord(i)
				length += 1

		#Close out the byte array and add the size as a variable
		output += "\n};\n"
		output += "unsigned int %s_len = %d;" % (array_name, length)
	
	#Return the output
	return output


## Main
if __name__ == "__main__":
	if len(sys.argv) < 2 or not os.path.exists(sys.argv[1]):
		print(sys.stderr, "The file doesn't exist or no path was given.")
		sys.exit(1)
	print(xxd(sys.argv[1]))

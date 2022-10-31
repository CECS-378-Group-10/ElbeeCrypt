## Enum Utilities ##


## Imports
from enum import Enum

## Functions
def enumNamesToString(enums: list[Enum]) -> str:
	"""
	Converts a list of enums to a list string.

	Args:
		enums (list[Enum]): The enum list to convert

	Returns:
		list[str]: The resultant string list
	"""
	
	#Create a string to store the names
	names: str = ""

	#Loop over the enums
	for i in range(0, len(enums)):
		#Add the current item to the string list
		names += str(enums[i].name)
		if i < len(enums) - 1: names += ", "

	#Return the list of names
	return names

def excludeUnknowns(enums: list(Enum)) -> list(Enum):
	"""
	Removes items from an enum list that have a key with
	the name "UNKNOWN". Does not mutate the input list.

	Args:
		enums (list): The list to modify

	Returns:
		list[Enum]: The modified list
	"""	

	#Clone the input list to maintain immutability
	_enums: list[Enum] = enums.copy()

	#Loop over the enum values
	for i in range(0, len(_enums)):
		#Remove the item if it has a name of unknown
		if str(_enums[i].name) == "UNKNOWN":
			del _enums[i]

	#Return the modified list
	return _enums
## Mutable enum class ##


## Imports
from __future__ import annotations
from enum import Enum

## Class definition
class MutableEnum(object):
	"""
	Represents a mutable enum. By default, enums in
	Python are immutable, and do not update the object
	when, for example, changed in a function. This class
	allows enums to be passed by reference and modified 
	by the function it was passed to.
	"""	

	#Fields
	data: Enum
	""" The content of the mutable enum class. """

	#Constructor
	def __init__(self, data: Enum):
		"""
		Constructs a new `MutableEnum` object.

		Args:
			data (str): The initial content of the object
		"""		
		self.data = data

	#Getters and setters
	def getData(self) -> Enum:
		"""
		Gets the underlying data of this object.

		Returns:
			str: The content of this object
		"""		
		return self.data
	
	def setData(self, data: Enum):
		"""
		Sets the underlying data of this object.

		Args:
			data (Enum): The new content of this object
		"""		
		self.data = data

	#Methods
	def equals(self, other: MutableEnum) -> bool:
		"""
		Checks if a given mutable enum equals this mutable enum.

		Args:
			other (Enum): The mutable enum to check against this object

		Returns:
			bool: Whether the two mutable enum objects are equal
		"""

		return self.equals(other.data)

	def equals(self, other: Enum) -> bool:
		"""
		Checks if a given enum equals this mutable enum.

		Args:
			other (str): The enum to check against this object

		Returns:
			bool: Whether the enum and this mutable enum are equal
		"""

		return self.data == other

	#toString overload
	def __repr__(self) -> str:
		"""
		Returns the string representation of this object.

		Returns:
			str: The string representation of this object
		"""		
		return self.data.name + " = " + self.data.value
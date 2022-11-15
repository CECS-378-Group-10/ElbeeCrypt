#pragma once

#include <iomanip>
#include <sstream>
#include <string>

/**
 * @brief Contains a set of utilities for working with strings from 
 * the C++ STL, eg: `vector`, `map`, etc. These are stateless methods 
 * and are not meant to be attached to one particular instance of an 
 * object. Credits are given where needed for 3rd party contributions.
 * 
 * @author CECS-378 Group 10
 * @file string.hpp
 */
namespace elbeecrypt::common::utils::String {
	/**
	 * @brief Returns the string representation of a boolean.
	 * 
	 * @param boolean The boolean to stringify
	 * @return The boolean's value
	 */
	const std::string boolStr(bool boolean);

	/**
	 * @brief Checks if a string contains another string.
	 * 
	 * @param str The string to look in
	 * @param target The target string to look for
	 * @param ignoreCase Whether case should be ignored when searching
	 * @return Whether the string contans the target
	 */
	bool contains(const std::string& str, const std::string& target, bool ignoreCase);

	/**
	 * @brief Checks if a string contains another string. Assumes that 
	 * the check is case sensitive.
	 * 
	 * @param str The string to look in
	 * @param target The target string to look for
	 * @return Whether the string contans the target
	 */
	bool contains(const std::string& str, const std::string& target);

	/**
	 * @brief Checks if a string contains a specific character.
	 * 
	 * @param str The string to look in
	 * @param target The target character to look for
	 * @param ignoreCase Whether case should be ignored when searching
	 * @return Whether the string contans the target
	 */
	bool contains(const std::string& str, const char& target, bool ignoreCase);

	/**
	 * @brief Checks if a string contains a specific character. Assumes 
	 * that the check is case sensitive.
	 * 
	 * @param str The string to look in
	 * @param target The target character to look for
	 * @return Whether the string contans the target
	 */
	bool contains(const std::string& str, const char& target);

	/**
	 * @brief Returns the first index of a char in a string. Returns 
	 * -1 if the char isn't in the string.
	 * 
	 * @param str The string to check
	 * @param target The target character to look for
	 * @return The first index of the character in the string, else -1 if it is not present
	 */
	int firstIndexOf(const std::string& str, const char& target);

	/**
	 * @brief Converts an integer into a hexadecimal string.
	 * See https://stackoverflow.com/a/50517242
	 * 
	 * @author Loss Mentality
	 * @tparam T The datatype of the integer
	 * @param integer The integer to convert
	 * @param format Whether a "0x" should be applied to the hexadecimal digit
	 * @return The resultant string
	 */
	template<typename T>
	std::string intToHex(T integer, bool format){
		//Ensure this function is called with a template parameter that makes sense. Note: static_assert is only available in C++11 and higher.
		static_assert(std::is_integral<T>::value, "Template argument 'T' must be a fundamental integer type (e.g. int, short, etc..).");

		//Create the sstream object to hold the result
		std::stringstream stream;

		//Add the necessary formatting
		if(format) stream << "0x";
		stream << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex;

		/*
			If T is an 8-bit integer type (e.g. uint8_t or int8_t) it will be
			treated as an ASCII code, giving the wrong result. So we use C++17's
			"if constexpr" to have the compiler decides at compile-time if it's
			converting an 8-bit int or not.
		*/
		if constexpr (std::is_same_v<std::uint8_t, T>){
			/*
				Unsigned 8-bit unsigned int type. Cast to int (thanks Lincoln) to
				avoid ASCII code interpretation of the int. The number of hex digits
				in the  returned string will still be two, which is correct for 8 bits,
				because of the 'sizeof(T)' above.
			*/
			stream << static_cast<int>(integer);
		}

		//Check for a signed 8-bit integer
		else if(std::is_same_v<std::int8_t, T>){
			/*
				For 8-bit signed int, same as above, except we must first cast to unsigned
				int, because values above 127d (0x7f) in the int will cause further issues.
				if we cast directly to int.
			*/
			stream << static_cast<int>(static_cast<uint8_t>(integer));
		}

		//No cast needed for ints wider than 8 bits.
		else {
			stream << integer;
		}

		//Return the hexadecimal digit
		return stream.str();
	}

	/**
	 * @brief Converts an integer into a hexadecimal string.
	 * Assumes that a "0x" should be pre-appended to the digit.
	 * See https://stackoverflow.com/a/50517242
	 * 
	 * @author Loss Mentality
	 * @tparam T The datatype of the integer
	 * @param integer The integer to convert
	 * @return The resultant string
	 */
	template<typename T>
	std::string intToHex(T integer){
		return intToHex(integer, true);
	}

	/**
	 * @brief Returns the last index of a char in a string. Returns 
	 * -1 if the char isn't in the string.
	 * 
	 * @param str The string to check
	 * @param target The target character to look for
	 * @return The last index of the character in the string, else -1 if it is not present
	 */
	int lastIndexOf(const std::string& str, const char& target);

	/**
	 * @brief Transforms a string to lowercase.
	 * 
	 * @param str The string to transform
	 * @return The string transformed to lowercase.
	 */
	const std::string toLowercase(const std::string& str);

	/**
	 * @brief Transforms a string to uppercase.
	 * 
	 * @param str The string to transform
	 * @return The string transformed to uppercase.
	 */
	const std::string toUppercase(const std::string& str);
}

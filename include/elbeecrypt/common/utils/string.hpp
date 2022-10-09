#pragma once

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
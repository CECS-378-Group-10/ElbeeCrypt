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
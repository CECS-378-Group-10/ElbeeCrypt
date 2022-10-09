#pragma once

#include <algorithm>
#include <string>
#include <vector>

/**
 * @brief Contains a set of utilities for working with containers
 * from the C++ STL, eg: `vector`, `map`, etc. These are stateless 
 * methods and are not meant to be attached to one particular instance
 * of an object. Credits are given where needed for 3rd party contributions.
 * 
 * @author CECS-378 Group 10
 * @file container.hpp
 */
namespace elbeecrypt::common::utils::Container {
	/**
	 * @brief Gets the total number of elements present in a 
	 * group of vectors. Do keep in mind that each vector must
	 * carry the same datatype, ie: if one vector is a vector
	 * of strings, all other vectors must also be vectors of
	 * strings.
	 * 
	 * @tparam T The datatype of the vectors
	 * @param vectors The vectors to sum up
	 * @return The total number of items across all vectors involved
	 */
	template<typename T>
	size_t combinedVectorSize(std::initializer_list<std::vector<T>> vectors){ //Defined here or else the linker will fail the build
		//Create the output size variable
		size_t total = 0;

		//Loop over the varargs
		for(std::vector<T> vector : vectors){
			//Add the current vector's size to the total
			total += vector.size();
		}

		//Return the total
		return total;
	}

	/**
	 * @brief Combines multiple vectors into one bigger vector.
	 * This bigger vector carries the combined size of all the
	 * member vectors. Do keep in mind that each vector must
	 * carry the same datatype, ie: if one vector is a vector
	 * of strings, all other vectors must also be vectors of
	 * strings.
	 * 
	 * @tparam T The datatype of the vectors
	 * @param vectors The vectors to combine
	 * @return The resultant vector
	 */
	template<typename T>
	std::vector<T> concatVectors(std::initializer_list<std::vector<T>> vectors){ //Defined here or else the linker will fail the build
		//Create the output vector and reserve space
		std::vector<T> out = {};
		out.reserve(combinedVectorSize(vectors));

		//Loop over the varargs
		for(std::vector<T> vector : vectors){
			//Dump the contents of the current vector into the output vector
			out.insert(out.end(), vector.begin(), vector.end());
		}

		//Return the output vector
		return out;
	}

	/**
	 * @brief Checks if a given vector contains a given item.
	 * 
	 * @tparam T The datatype of the vector and the item to look for
	 * @param vec The vector to search
	 * @param target The item to search for
	 * @return Whether the vector contains the item
	 */
	template<typename T>
	bool contains(const std::vector<T>& vec, const T& target){
		return std::find(vec.begin(), vec.end(), target) != vec.end();
	}

	/**
	 * @brief Splits an input string up by a given regex. Based on a 
	 * method described here: https://www.techiedelight.com/split-string-cpp-using-delimiter/
	 * 
	 * @author TechieDelight
	 * @param str The string to split up
	 * @param regexp The regex string to use on the input string
	 * @return The split up string as a vector
	 */
	std::vector<std::string> tokenize(const std::string& str, const std::string& regexp);
}
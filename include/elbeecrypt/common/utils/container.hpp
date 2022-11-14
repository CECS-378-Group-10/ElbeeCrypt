#pragma once

#include <algorithm>
#include <array>
#include <filesystem>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "elbeecrypt/common/utils/string.hpp"

namespace fs = std::filesystem;

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
	 * @brief Converts a C-style array of integers to a string.
	 * 
	 * @tparam T The datatype of the array
	 * @param arr The array to convert
	 * @param size The size of the array
	 * @param format Whether to add brackets, commas, and "0x" to the items
	 * @return The string representation of the array
	 */
	template<typename T>
	std::string cIntArrayToStr(T arr[], size_t size, bool format){
		//Ensure this function is called with an array of integers
		static_assert(std::is_integral<T>::value, "Template argument 'T' must be a fundamental integer type (e.g. int, short, etc..).");

		//Create the string object
		std::string out = "";
		if(format) out += "[";

		//Add each element to the string
		for(size_t i = 0; i < size; i++){
			out += elbeecrypt::common::utils::String::intToHex(arr[i], format);
			if(format && i < size - 1) out += ", ";
		}

		//Return the string
		if(format) out += "]";
		return out;
	}

	/**
	 * @brief Converts a C-style array of integers to a string.
	 * Assumes that the output string representation should be 
	 * formatted and have "0x" appended to the items.
	 * 
	 * @tparam T The datatype of the array
	 * @param arr The array to convert
	 * @param size The size of the array
	 * @return The string representation of the array
	 */
	template<typename T>
	std::string cIntArrayToStr(T arr[], size_t size){
		return cIntArrayToStr(arr, size, true);
	}

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
	bool contains(const std::vector<T>& vec, const T& target){ //Defined here or else the linker will fail the build
		return std::find(vec.begin(), vec.end(), target) != vec.end();
	}

	/**
	 * @brief Checks if a given vector contains a given string.
	 * 
	 * @param vec The vector to search
	 * @param target The string to search for
	 * @param ignoreCase Whether case should be ignored
	 * @return Whether the vector contains the string
	 */
	bool contains(const std::vector<std::string>& vec, const std::string& target, bool ignoreCase);

	/**
	 * @brief Checks if a given vector contains a given string. Assumes that the
	 * search is case sensitive
	 * 
	 * @param vec The vector to search
	 * @param target The string to search for
	 * @return Whether the vector contains the string
	 */
	bool contains(const std::vector<std::string>& vec, const std::string& target);

	/**
	 * @brief Converts an `std::array` of integers to a string.
	 * 
	 * @tparam T The datatype of the array
	 * @tparam size The size of the array. Wil be determined by the object passed in
	 * @param arr The array to convert
	 * @param format Whether to add brackets, commas, and "0x" to the items
	 * @return The string representation of the array
	 */
	template<typename T, size_t size>
	std::string intArrayToStr(const std::array<T, size>& arr, bool format){
		return cIntArrayToStr(arr.data(), arr.size());
	}

	/**
	 * @brief Converts an `std::array` of integers to a string.
	 * Assumes that the output string representation should be 
	 * formatted and have "0x" appended to the items.
	 * 
	 * @tparam T The datatype of the array
	 * @tparam size The size of the array. Wil be determined by the object passed in
	 * @param arr The array to convert
	 * @return The string representation of the array
	 */
	template<typename T, size_t size>
	std::string intArrayToStr(const std::array<T, size>& arr){
		return cIntArrayToStr(arr.data(), arr.size(), true);
	}

	/**
	 * @brief Removes duplicate elements from a vector using a set.
	 * See: https://stackoverflow.com/a/31748822
	 * 
	 * @author yury
	 * @tparam T The datatype of the elements
	 * @param vec THe vector to remove the duplicates from
	 */
	template<typename T>
	void removeDuplicates(std::vector<T>& vec){
		std::set<T> values;
		vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const T& value){ 
			return !values.insert(value).second; 
		}), vec.end());
	}

	/**
	 * @brief Splits up a vector into equally sized portions and
	 * inserts each portion into a map, including where the vector
	 * shard is in relation to its source vector. Useful for 
	 * applications where one may want to share the contents of
	 * the vector across multiple threads to speed up the 
	 * processing times. Do note that this should only be done
	 * if the order of the contents does not matter in the 
	 * processor in which this function is to be used. 
	 * See https://stackoverflow.com/a/37708514
	 * 
	 * @author Yury
	 * @tparam T The datatype of the vector items
	 * @param target The target vector to derive the elements from
	 * @param shardCount How many ways to split the source vector
	 * @return A map containing the vector shard's first element's position in the parent and the shard itself
	 */
	template<typename T>
	std::map<uint32_t, std::vector<T>> shardVector(const std::vector<T>& target, uint32_t shardCount){
		//Ensure that the shard count is greater than 0
		if(shardCount < 1) throw std::runtime_error("Argument \"shardCount\" must be greater than or equal to 1.");

		//Get the number of elements per shard and the remainder
		size_t elementsPerShard = target.size() / shardCount;
		size_t elementsRemaining = target.size() % shardCount;

		//Create the output map
		std::map<uint32_t, std::vector<T>> out = {};

		//Loop over the shard count
		size_t begin = 0;
		size_t end = 0;
		for(size_t i = 0; i < std::min(shardCount, (uint32_t) target.size()); i++){
			//Get the current end offset
			end += (elementsRemaining > 0) ? (elementsPerShard + !!(elementsRemaining--)) : elementsPerShard;

			//Create the vector shard
			std::vector<T> shard(target.begin() + begin, target.begin() + end);

			//Insert the vector shard and assign it as a value for the current i
			out.insert_or_assign(i, shard);

			//Set the beginning offset to be the current end offset
			begin = end;
		}

		//Return the resulting map
		return out;
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

	/**
	 * @brief Undoes the vector shard operation performed by the 
	 * shard function by combining all of the vectors into one
	 * big vector.
	 * 
	 * @tparam T The datatype of the vector items
	 * @param target The target vector shard map
	 * @return The resultant recombined vector
	 */
	template<typename T>
	std::vector<T> unshardVector(const std::map<uint32_t, std::vector<T>>& target){
		//Create a new vector to store the shards
		std::vector<T> unsharded = {};

		//Loop over the shard map
		for(auto shardPair : target){
			//Get the current shard item
			std::vector<T> shard = shardPair.second;

			//Push the vector's items into the unsharded vector
			unsharded.insert(std::end(unsharded), std::begin(shard), std::end(shard));
		}

		//Return the unsharded vector
		return unsharded;
	}

	/**
	 * @brief Converts a vector of items to a string version
	 * of the vector.
	 * 
	 * @param vec The vector to convert
	 * @param converter The lambda used to convert the item to a string
	 * @return The resultant string
	 */
	template<typename T>
	std::string vecStr(const std::vector<T>& vec, std::function<std::string(const T&)> converter){
		//Create a string to hold the contents of the data to be outputted
		std::string out = "[";

		//Loop over the vector
		for(size_t i = 0; i < vec.size(); i++){
			if(i > 0) out += ", "; //Add a delimiter if i > 0
			out += converter(vec.at(i));
		}

		//Add the right bracket and return
		out += "]";
		return out;
	}

	/**
	 * @brief Converts a vector of strings to a string version
	 * of the vector.
	 * 
	 * @param vec The vector to convert
	 * @return The resultant string
	 */
	std::string vecStr(const std::vector<std::string>& vec);

	/**
	 * @brief Converts a vector of paths to a string version
	 * of the vector.
	 * 
	 * @param vec The vector to convert
	 * @return The resultant string
	 */
	std::string vecPathStr(const std::vector<fs::path>& vec);
}

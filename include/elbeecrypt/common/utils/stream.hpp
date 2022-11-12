#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

/**
 * @brief Contains a set of utilities for working with streams from 
 * the C++ STL, eg: `ifstream`, `ostream`, etc. These are stateless 
 * methods  and are not meant to be attached to one particular instance 
 * of an object. Credits are given where needed for 3rd party contributions.
 * 
 * @author CECS-378 Group 10
 * @file stream.hpp
 */
namespace elbeecrypt::common::utils::Stream {
	/**
	 * @brief Loads a file into an `std::fstream` object for processing.
	 * Includes logic to determine the cause of errors and can intelligently
	 * determine the type of the `fstream` via the use of `std::is_same`.
	 * 
	 * @tparam T The datatype of the input stream
	 * @param fileStream The stream to load
	 * @param path The path to the file to load
	 * @param openMode The mode to open the file in
	 * @param allowDirectory Whether directories are allowed to be loaded
	 * @return Whether the load succeeded or failed
	 */
	template<class T>
	bool loadFStream(T &fileStream, const fs::path& path, const std::ios_base::openmode& openMode, const bool allowDirectory){
		//Ensure the input template argument is an fstream
		static_assert(
			std::is_same<std::ifstream, T>::value || std::is_same<std::ofstream, T>::value,
			"Template argument 'T' must be an fstream object, ie: 'std::ifstream' or 'std::ofstream'."
		);

		//Load the path into the fstream object
		fileStream.open(path.string(), openMode);

		//Check if the load was successful and the path doesn't point to a file
		if(fileStream.is_open() && fs::is_directory(path) && allowDirectory) return true;

		//Determine what went wrong with the open process
		std::string errorName;

		//Check 1 & 2: file does not exist or points to a directory
		if(!fs::exists(path)) errorName = "NONEXISTANT FILE";
		if(!allowDirectory && fs::is_directory(path)) errorName = "DIRECTORY";

		//Check 3: file permissions
		fs::perms perms = fs::status(path).permissions();

		//Check 3a: read permissions (only for std::ifstream)
		if(std::is_same<std::ifstream, T>::value){
			if(
				(perms & fs::perms::owner_read) == fs::perms::none ||
				(perms & fs::perms::group_read) == fs::perms::none ||
				(perms & fs::perms::others_read) == fs::perms::none
			) errorName = "NO READ PERMS (std::ifstream)";
		}

		//Check 3b: write permissions (only for std::ofstream)
		if(std::is_same<std::ofstream, T>::value){
			if(
				(perms & fs::perms::owner_write) == fs::perms::none ||
				(perms & fs::perms::group_write) == fs::perms::none ||
				(perms & fs::perms::others_write) == fs::perms::none
			) errorName = "NO WRITE PERMS (std::ofstream)";
		}
		
		//If no obvious error name exists, switch over the rdstate of the stream
		if(errorName.size() == 0){
			//See https://cplusplus.com/reference/ios/ios_base/iostate/
			switch(fileStream.rdstate()){
				case std::ios::badbit: //Read error during an I/O operation
					errorName = "READ ERROR";
					break;
				case std::ios::failbit: //Logical error during an I/O operation
					errorName = "LOGICAL ERROR";
					break;
				default: //Default state
					errorName = "<UNKNOWN>";
					break;
			}
		}

		//Construct and output the error
		std::cerr << "Encountered exception while reading file at path '" << path.string() << "': " << errorName << std::endl;
		return false;
	}

	/**
	 * @brief Loads a file into an `std::fstream` object for processing.
	 * Includes logic to determine the cause of errors and can intelligently
	 * determine the type of the `fstream` via the use of `std::is_same`.
	 * Assumes that directories are not allowed to be loaded.
	 * 
	 * @tparam T The datatype of the input stream
	 * @param fileStream The stream to load
	 * @param path The path to the file to load
	 * @param openMode The mode to open the file in
	 * @return Whether the load succeeded or failed
	 */
	template<class T>
	bool loadFStream(T &fileStream, const fs::path& path, const std::ios_base::openmode& openMode){
		return loadFStream(fileStream, path, openMode, false);
	}
}


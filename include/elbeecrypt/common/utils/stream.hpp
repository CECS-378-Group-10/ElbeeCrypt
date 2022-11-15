#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include "elbeecrypt/common/utils/fs.hpp"

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

		//If the path points to a directory when not allowed to do so, mark a status boolean as true
		bool illegalDirectoryPath = false;
		if(!allowDirectory && fs::is_directory(path)) illegalDirectoryPath = true;

		//Load the path into the fstream object
		fileStream.open(path.string(), openMode);

		//Check if the load was successful
		if(fileStream.is_open() && !illegalDirectoryPath) return true;

		//Create an error handler lambda
		auto handler = [&path](const std::string& errorName){
			std::cerr << "Encountered exception while reading file at path '" << path.string() << "': " << errorName << std::endl;
			return false; //Always return false
		};

		//Check 1 & 2: file does not exist or points to a directory
		if(!fs::exists(path)) return handler("NONEXISTANT FILE");
		if(illegalDirectoryPath) return handler("DIRECTORY NOT ALLOWED");

		//Check 3: file permissions
		fs::perms perms = fs::status(path).permissions();

		//Check 3a: read permissions (only for std::ifstream)
		if(std::is_same<std::ifstream, T>::value){
			if(!elbeecrypt::common::utils::FS::hasFullRead(perms))
				return handler("NO READ PERMS (std::ifstream)");
		}

		//Check 3b: write permissions (only for std::ofstream)
		if(std::is_same<std::ofstream, T>::value){
			if(!elbeecrypt::common::utils::FS::hasFullWrite(perms))
				return handler("NO WRITE PERMS (std::ofstream)");
		}
		
		//If no obvious error name exists, switch over the rdstate of the stream
		//See https://cplusplus.com/reference/ios/ios_base/iostate/
		switch(fileStream.rdstate()){
			case std::ios::badbit: //Read error during an I/O operation
				return handler("READ ERROR");
			case std::ios::failbit: //Logical error during an I/O operation
				return handler("LOGICAL ERROR");
			default: //Default state
				break;
		}

		//Default state
		return handler("<UNKNOWN>");
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

		/**
	 * @brief Writes a vector of items to a file.
	 * 
	 * @tparam The datatype of the items in the vector
	 * @param target The path to the file
	 * @param content The content to write to the file
	 * @param converter The lambda used to convert the item to a string
	 * @return Whether the write succeeded
	 */
	template<typename T>
	bool writeToFile(const fs::path& target, const std::vector<T>& content, std::function<std::string(const T&)> converter){
		//Open the file using the safe stream API
		std::ofstream file;
		bool opened = loadFStream(file, target, std::ios_base::out);

		//If the load failed, exit early
		if(!opened) return false;

		//Loop over the content vector
		for(T item : content){
			//Get the string representation of the current item using the converter lambda
			std::string line = converter(item);

			//Write the line to the file
			file << line << std::endl;
		}

		//Close the file and return true
		file.close();
		return true;
	}

	/**
	 * @brief Writes a vector of strings to a file.
	 * 
	 * @param target The path to the file
	 * @param lines The lines to write to the file
	 * @return Whether the write succeeded
	 */
	bool writeToFile(const fs::path& target, const std::vector<std::string>& lines);

	/**
	 * @brief Writes a vector of paths to a file.
	 * 
	 * @param target The path to the file
	 * @param paths The paths to write to the file
	 * @return Whether the write succeeded
	 */
	bool writeToFile(const fs::path& target, const std::vector<fs::path>& paths);

	/**
	 * @brief Writes a single line to a file.
	 * 
	 * @param target The path to the file
	 * @param line The line to write to the file
	 * @return Whether the write succeeded
	 */
	bool writeToFile(const fs::path& target, const std::string& line);
}

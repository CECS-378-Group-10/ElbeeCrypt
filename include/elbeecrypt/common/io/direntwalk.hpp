#pragma once

#include <filesystem>
#include <functional>
#include <vector>

namespace fs = std::filesystem;

/**
 * @brief Contains the directory walking API along with multiple
 * implementations for common operations using it. The main 
 * function in this library, `walk()`, contains the bulk of the
 * functionality. More can be done using this base function by
 * passing in lambdas (anonymous functions) that can do more with
 * the results than the base function or included implementations
 * can provide. These lambdas should be declared with the following
 * header: 
 * 
 * @author CECS-378 Group 10
 * @file direntwalk.hpp
 */
namespace elbeecrypt::common::io::DirentWalk {
	/**
	 * @brief Performs a recursive directory listing, collecting all of
	 * the found files in a vector. Relies on the `walk()` function to
	 * perform the necessary work.
	 * 
	 * @param root The root path to start from
	 * @param paths The vector to which the paths will be written
	 */
	void directoryList(const fs::path& root, std::vector<fs::path>& paths);

	/**
	 * @brief Returns the current directory that the program is 
	 * running in context to.
	 * 
	 * @return The path the program is running in
	 */
	fs::path pwd();

	/**
	 * @brief Walks a directory recursively using POSIX's `dirent.h` API.
	 * This function, by default, does not do anything to the directory 
	 * listings generated. That functionality must be provided via a lambda
	 * called `fileConsumer`. A single path is sent to the lambda for processing,
	 * returning void (no return) when complete. A second lambda called
	 * `folderConsumer` is also available which allows control over whether
	 * a directory is processed or not. If the lambda returns true, then
	 * the directory is processed. If not, it is ignored and the walker
	 * moves on. Adapted from the following Stack Overflow thread: 
	 * https://stackoverflow.com/a/612176
	 * 
	 * @param root The root path to start from
	 * @param fileConsumer The callback lambda to utilize whenever a file is encountered. Doesn't process directories
	 * @param folderConsumer The callback lambda to utilize whenever a folder is encountered
	 */
	void walk(const fs::path& root, std::function<void(const fs::path&)> fileConsumer, std::function<bool(const fs::path&)> folderConsumer);
}
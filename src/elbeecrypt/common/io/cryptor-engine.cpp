#include "elbeecrypt/common/io/cryptor-engine.hpp"
#include "elbeecrypt/common/utils/threadsafe_cerr.hpp"

namespace elbeecrypt::common::io {
	//Include namespace definitions
	using namespace elbeecrypt::common::utils;

	//Non-enum private constants
	/** Impl of CIPHER_ALGO. */
	const std::string CryptorEngine::CIPHER_ALGO = "XChaCha20-Poly1305";

	/** Impl of KEY_TYPE. */
	const std::string CryptorEngine::KEY_TYPE = "Curve25519";


	//Constructors & Destructor
	/** Impl of CryptorEngine(array<uint8_t>, uint32_t). */
	CryptorEngine::CryptorEngine(const std::array<uint8_t, KEY_SIZE>& skey, const uint32_t& chunkSize): _chunkSize(chunkSize), cipheredChunkSize(calculateCipheredChunkSize(chunkSize)){
		//Load the key from the given argument, initialize LibSodium, and lock its memory location
		key = skey;
		init();
		lockKeyMem(key);
	}

	/** Impl of CryptorEngine(array<uint8_t>). */
	CryptorEngine::CryptorEngine(const std::array<uint8_t, KEY_SIZE>& skey): CryptorEngine(skey, DEFAULT_PT_BUF_SIZE){} //Redirects to Constructor 1a

	/** Impl of CryptorEngine(path, uint32_t). */
	CryptorEngine::CryptorEngine(const fs::path& keyPath, const uint32_t& chunkSize): _chunkSize(chunkSize), cipheredChunkSize(calculateCipheredChunkSize(chunkSize)){ //Constructor 2a impl
		//Load the key from the given file, initialize LibSodium, and lock its memory location
		key = importKey(keyPath);
		init();
		lockKeyMem(key);
	}

	/** Impl of CryptorEngine(path). */
	CryptorEngine::CryptorEngine(const fs::path& keyPath): CryptorEngine(keyPath, DEFAULT_PT_BUF_SIZE){} //Redirects to Constructor 2a

	/** Impl of CryptorEngine(uint32_t). */
	CryptorEngine::CryptorEngine(const uint32_t& chunkSize): _chunkSize(chunkSize), cipheredChunkSize(calculateCipheredChunkSize(chunkSize)){ //Constructor 3a impl
		//Initialize LibSodium, generate the symmetric key, and lock its memory location
		init();
		crypto_secretstream_xchacha20poly1305_keygen(key.data());
		lockKeyMem(key);
	}

	/** Impl of CryptorEngine(). */
	CryptorEngine::CryptorEngine(): CryptorEngine(DEFAULT_PT_BUF_SIZE){} //Redirects to Constructor 3a

	/** Impl of ~CryptorEngine(). */
	CryptorEngine::~CryptorEngine(){
		//Zero out the key array to securely erase it
		//See: https://libsodium.gitbook.io/doc/memory_management
		sodium_memzero(key.data(), key.size());

		//Unlock the memory location where the key is held
		unlockKeyMem(key);
	}


	//Getters
	/** Impl of getKey(). */
	const std::array<uint8_t, CryptorEngine::KEY_SIZE>& CryptorEngine::getKey() const {
		return key;
	}

	/** Impl of getChunkSize(). */
	uint32_t CryptorEngine::getChunkSize(){
		return _chunkSize;
	}

	/** Impl of getCipheredChunkSize(). */
	uint32_t CryptorEngine::getCipheredChunkSize(){
		return cipheredChunkSize;
	}


	//Methods
	/** Impl of decryptFile(path, path). */
	CryptorEngine::Status CryptorEngine::decryptFile(const fs::path& src, const fs::path& dest){
		//Initialize the I/O streams
		std::ifstream ciphertext;
		std::ofstream plaintext;
		if(!Stream::loadFStream(ciphertext, src, std::ios::binary | std::ios::in)) return CryptorEngine::Status::FAIL;
		if(!Stream::loadFStream(plaintext, dest, std::ios::binary | std::ios::out)) return CryptorEngine::Status::FAIL;

		//Initialize the I/O buffers and the stream header (declared on the heap to stop GCC from warning)
		uint8_t* ciphertextBuf = new uint8_t[cipheredChunkSize];
		uint8_t* plaintextBuf = new uint8_t[_chunkSize];
		uint8_t* headerBuf = new uint8_t[H_BUF_SIZE];

		//Initialize the stream cipher state as well as the tag
		crypto_secretstream_xchacha20poly1305_state state;
		uint8_t tag;
		const std::string errorPrefix = "ERROR while decrypting file at path '" + src.string() + "': ";

		//Initialize the chunk flags
		size_t rlen; //The length of the chunk to read
		uint64_t wlen; //The length of the chunk to write
		int eof; //Whether the current chunk is the last one to process

		//Check if the ciphertext header is intact
		ciphertext.read((char*) headerBuf, H_BUF_SIZE);
		if(crypto_secretstream_xchacha20poly1305_init_pull(&state, headerBuf, key.data()) != 0){
			//Incomplete header
			Cerr{} << errorPrefix << "Incomplete header" << std::endl;
			cleanup(ciphertext, plaintext, plaintextBuf, ciphertextBuf, headerBuf);
			return Status::FAIL;
		}

		//Begin decrypting the file
		size_t index = 0;
		do {
			//Read in the current chunk and get the EOF state
			ciphertext.read((char*) ciphertextBuf, cipheredChunkSize);
			rlen = ciphertext.gcount(); //How many bytes were read
			eof = ciphertext.eof(); //Whether an EOF was encountered

			//Decrypt the current ciphertext chunk and check if the MAC checks out
			/*
				Note: `uint64_t` and `unsigned long long` have the same size (64-bits), but 64-bit Linux GCC (LP64) defines it as `unsigned long int`.
				`unsigned long int` is a 64-bit type on 64-bit Linux GCC (LP64) despite the type being 32-bit on other platforms.
				The `reinterpret_cast` is, therefore, safe on LP64 GCC and seemingly so on other compilers too.
				Further reading: https://en.cppreference.com/w/cpp/language/types#Properties
			*/
			if(crypto_secretstream_xchacha20poly1305_pull(&state, plaintextBuf, &reinterpret_cast<unsigned long long&>(wlen), &tag, ciphertextBuf, rlen, NULL, 0) != 0){
				//Corrupted chunk
				Cerr{} << errorPrefix << "Corrupt chunk @ pos " << index << std::endl;
				cleanup(ciphertext, plaintext, plaintextBuf, ciphertextBuf, headerBuf);
				return Status::FAIL;
			}

			//Check if the stream terminates prematurely
			if(tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !eof){
				//Premature end (end of file reached before the end of the stream)
				Cerr{} << errorPrefix << "Premature EOF @ pos " << index << std::endl;
				cleanup(ciphertext, plaintext, plaintextBuf, ciphertextBuf, headerBuf);
				return Status::FAIL;
			}

			//Write the decrypted chunk to the plaintext buffer
			plaintext.write((char*) plaintextBuf, (size_t) wlen);

			//Increment the index
			index++;
		} while(!eof);

		//Cleanup the streams and buffers then return 0
		cleanup(ciphertext, plaintext, plaintextBuf, ciphertextBuf, headerBuf);
		return Status::OK;
	}

	/** Impl of encryptFile(path, path). */
	CryptorEngine::Status CryptorEngine::encryptFile(const fs::path& src, const fs::path& dest){
		//Initialize the I/O streams
		std::ifstream plaintext;
		std::ofstream ciphertext;
		if(!Stream::loadFStream(plaintext, src, std::ios::binary | std::ios::in)) return CryptorEngine::Status::FAIL;
		if(!Stream::loadFStream(ciphertext, dest, std::ios::binary | std::ios::out)) return CryptorEngine::Status::FAIL;

		//Initialize the I/O buffers and the stream header (declared on the heap to stop GCC from warning)
		uint8_t* plaintextBuf = new uint8_t[_chunkSize];
		uint8_t* ciphertextBuf = new uint8_t[cipheredChunkSize];
		uint8_t* headerBuf = new uint8_t[H_BUF_SIZE];

		//Initialize the stream cipher state as well as the tag
		crypto_secretstream_xchacha20poly1305_state state;
		uint8_t tag;

		//Initialize the chunk flags
		size_t rlen; //The length of the chunk to read
		uint64_t wlen; //The length of the chunk to write
		int eof; //Whether the current chunk is the last one to process

		//Create the ciphertext header and write it to the ciphertext stream
		crypto_secretstream_xchacha20poly1305_init_push(&state, headerBuf, key.data());
		ciphertext.write((char*) headerBuf, H_BUF_SIZE);

		//Begin encrypting the file
		size_t index = 0;
		do {
			//Read in the current chunk
			plaintext.read((char*)  plaintextBuf, _chunkSize);
			rlen = plaintext.gcount(); //How many bytes were read

			//Calculate the stream cipher tag value based on the ifstream's EOF state
			eof = plaintext.eof(); //Whether an EOF was encountered
			tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;

			//Encrypt the current plaintext chunk and write it to the ciphertext buffer
			/*
				Note: `uint64_t` and `unsigned long long` have the same size (64-bits), but 64-bit Linux GCC (LP64) defines it as `unsigned long int`.
				`unsigned long int` is a 64-bit type on 64-bit Linux GCC (LP64) despite the type being 32-bit on other platforms.
				The `reinterpret_cast` is, therefore, safe on LP64 GCC and seemingly so on other compilers too.
				Further reading: https://en.cppreference.com/w/cpp/language/types#Properties
			*/
			crypto_secretstream_xchacha20poly1305_push(&state, ciphertextBuf, &reinterpret_cast<unsigned long long&>(wlen), plaintextBuf, rlen, NULL, 0, tag);
			ciphertext.write((char*) ciphertextBuf, (size_t) wlen);

			//Increment the index
			index++;
		} while(!eof);

		//Cleanup the streams and buffers then return 0
		cleanup(plaintext, ciphertext, ciphertextBuf, plaintextBuf, headerBuf);
		return Status::OK;
	}

	/** Impl of exportPrivkey(path). */
	bool CryptorEngine::exportKey(const fs::path& dest){
		return CryptorEngine::exportKey(key, dest);
	}
	
	/** Impl of keyFingerprint(). */
	std::string CryptorEngine::keyFingerprint(){
		return CryptorEngine::fingerprint(key);
	}

	/** Impl of toString(). */
	std::string CryptorEngine::toString(){
		//Create the output string object
		std::string out = "CryptorEngine{";

		//Add the relevant attributes
		out += "cipherSuite=" + CIPHER_ALGO + ", ";
		out += "keyType=" + KEY_TYPE + ", ";
		out += "keyFingerprint=" + keyFingerprint() + ", ";
		out += "chunkSize=" + std::to_string(_chunkSize) + ", ";
		out += "cipheredChunkSize=" + std::to_string(cipheredChunkSize);

		//Close off the output string and return it
		return out += "}";
	}


	//Operator overloads
	/** Impl of operator string(). */
	CryptorEngine::operator std::string(){
		return toString();
	}


	//Utility functions
	/** Impl of exportKey(array<uint8_t>, path). */
	bool CryptorEngine::exportKey(const std::array<uint8_t, KEY_SIZE>& key, const fs::path& dest){
		//Create the stream to write the key to
		std::ofstream keyStream;
		if(!elbeecrypt::common::utils::Stream::loadFStream(keyStream, dest, std::ios::binary | std::ios::out)) return false;

		//Write the key to the stream
		keyStream.write((char*) key.data(), key.size());

		//Return true, assuming everything else went okay
		keyStream.close();
		return true;
	}

	/** Impl of fingerprint(array<uint8_t>). */
	std::string CryptorEngine::fingerprint(const std::array<uint8_t, KEY_SIZE>& key){
		//Create the output array to store the digest bytes
		uint8_t hash[crypto_hash_sha256_BYTES];

		//Digest the key using SHA-256
		int digestRet = crypto_hash_sha256(hash, key.data(), key.size());
		if(digestRet != 0) return "";

		//Convert the digest array to a string and return it
		return elbeecrypt::common::utils::Container::cIntArrayToStr(hash, crypto_hash_sha256_BYTES, false);
	}

	/** Impl of importKey(path). */
	std::array<uint8_t, CryptorEngine::KEY_SIZE> CryptorEngine::importKey(const fs::path& src){
		//Create the stream to read the key from
		std::ifstream keyStream;
		if(!Stream::loadFStream(keyStream, src, std::ios::ate | std::ios::binary | std::ios::in))
			throw std::runtime_error("Could not load the key located at path: '" + src.string() + "'");

		//Ensure the file size is correct for a private key
		if(keyStream.tellg() != CryptorEngine::KEY_SIZE)
			throw std::runtime_error("Key size mismatch (" + std::to_string(keyStream.tellg()) + " bytes) for key located at path: '" + src.string() + "'");

		//Create an array to store the key
		std::array<uint8_t, CryptorEngine::KEY_SIZE> key;

		//Load the key into the array
		keyStream.seekg(0); //Make sure to do this before loading the key since the file was loaded in append mode to get the size
		keyStream.read((char*) key.data(), CryptorEngine::KEY_SIZE);

		//Return the key array
		return key;
	}


	//Private utility functions
	/** Impl of calculateCipheredChunkSize(uint32_t). */
	uint32_t CryptorEngine::calculateCipheredChunkSize(const uint32_t& chunkSize){
		return chunkSize + crypto_secretstream_xchacha20poly1305_ABYTES;
	}

	/** Impl of cleanup(ifstream, ofstream, uint8_t, uint8_t, uint8_t). */
	void CryptorEngine::cleanup(std::ifstream& in, std::ofstream& out, uint8_t* bufferA, uint8_t* bufferB, uint8_t* bufferC){
		//Close the streams
		in.close();
		out.close();

		//Close the buffers
		delete[] bufferA;
		delete[] bufferB;
		delete[] bufferC;
	}

	/** Impl of init(). */
	void CryptorEngine::init(){
		//Initialize LibSodium
		int sodiumInitRetVal = sodium_init();
		if(sodiumInitRetVal == -1) throw std::runtime_error("LibSodium could not be safely initialized. Return code: " + sodiumInitRetVal);
	}

	/** Impl of lockKeyMem(array<uint8_t>). */
	void CryptorEngine::lockKeyMem(std::array<uint8_t, KEY_SIZE>& key){
		//Lock the key's memory location
		int memLockRet = sodium_mlock(&key, key.size());
		if(memLockRet != 0) throw std::runtime_error("LibSodium could not lock the key's memory location. Return code: " + std::to_string(memLockRet));
	}

	/** Impl of unlockKeyMem(array<uint8_t>). */
	void CryptorEngine::unlockKeyMem(std::array<uint8_t, KEY_SIZE>& key){
		//Unlock the key's memory location
		int memUnlockRet = sodium_munlock(&key, key.size());
		if(memUnlockRet != 0) throw std::runtime_error("LibSodium could not unlock the key's memory location. Return code: " + std::to_string(memUnlockRet));
	}
}

#include "elbeecrypt/common/io/cryptor-engine.hpp"

namespace elbeecrypt::common::io {
	//Include namespace definitions
	using namespace elbeecrypt::common::utils;

	//Non-enum private constants
	/** Impl of CIPHER_ALGO. */
	const std::string CryptorEngine::CIPHER_ALGO = "XChaCha20-Poly1305";

	/** Impl of KEY_TYPE. */
	const std::string CryptorEngine::KEY_TYPE = "Curve25519";


	//Constructors & Destructor
	/** Impl of CryptorEngine(array, uint32_t). */
	CryptorEngine::CryptorEngine(std::array<uint8_t, PRIVKEY_SIZE>& key, const uint32_t& chunkSize): _chunkSize(chunkSize), cipheredChunkSize(calculateCipheredChunkSize(chunkSize)){ //Constructor 1a impl
		//Setup the keys
		setupFromPrivkey(key);
	}

	/** Impl of CryptorEngine(array). */
	CryptorEngine::CryptorEngine(std::array<uint8_t, PRIVKEY_SIZE>& key): CryptorEngine(key, DEFAULT_PT_BUF_SIZE){} //Redirects to Constructor 1a

	/** Impl of CryptorEngine(path, uint32_t). */
	CryptorEngine::CryptorEngine(const fs::path& keyPath, const uint32_t& chunkSize): _chunkSize(chunkSize), cipheredChunkSize(calculateCipheredChunkSize(chunkSize)){ //Constructor 2a impl
		//Load the key from the given file
		std::ifstream keyStream;
		if(!Stream::loadFStream(keyStream, keyPath, std::ios::ate | std::ios::binary | std::ios::in))
			throw std::runtime_error("Could not load the key located at path: '" + keyPath.string() + "'");

		//Ensure the file size is correct for a private key
		if(keyStream.tellg() != PRIVKEY_SIZE)
			throw std::runtime_error("Key size mismatch (" + std::to_string(keyStream.tellg()) + " bytes) for key located at path: '" + keyPath.string() + "'");

		//Create an array to store the key
		std::array<uint8_t, PRIVKEY_SIZE> key;

		//Load the key into the array
		keyStream.seekg(0); //Make sure to do this before loading the key since the file was loaded in append mode to get the size
		keyStream.read((char*) key.data(), PRIVKEY_SIZE);

		//Setup the keys
		setupFromPrivkey(key);
	}

	/** Impl of CryptorEngine(path). */
	CryptorEngine::CryptorEngine(const fs::path& keyPath): CryptorEngine(keyPath, DEFAULT_PT_BUF_SIZE){} //Redirects to Constructor 2a

	/** Impl of CryptorEngine(uint32_t). */
	CryptorEngine::CryptorEngine(const uint32_t& chunkSize): _chunkSize(chunkSize), cipheredChunkSize(calculateCipheredChunkSize(chunkSize)){ //Constructor 3a impl
		//Generate the public and private keys
		int genKeysRet = crypto_box_keypair(pubkey.data(), privkey.data());
		if(genKeysRet != 0) throw std::runtime_error("LibSodium could not generate the required keypair. Return code: " + std::to_string(genKeysRet));
	}

	/** Impl of CryptorEngine(). */
	CryptorEngine::CryptorEngine(): CryptorEngine(DEFAULT_PT_BUF_SIZE){} //Redirects to Constructor 3a

	/** Impl of ~CryptorEngine(). */
	CryptorEngine::~CryptorEngine(){
		//Zero out the key arrays to securely erase them
		//See: https://libsodium.gitbook.io/doc/memory_management
		sodium_memzero(privkey.data(), privkey.size());
		sodium_memzero(pubkey.data(), pubkey.size());
		sodium_memzero(sharedkey.data(), sharedkey.size());
	}


	//Getters
	/** Impl of getPrivkey(). */
	const std::array<uint8_t, CryptorEngine::PRIVKEY_SIZE>& CryptorEngine::getPrivkey() const {
		return privkey;
	}

	/** Impl of getPubkey(). */
	const std::array<uint8_t, CryptorEngine::PUBKEY_SIZE>& CryptorEngine::getPubkey() const {
		return pubkey;
	}

	/** Impl of getSharedkey(). */
	const std::array<uint8_t, CryptorEngine::SHAREDKEY_SIZE>& CryptorEngine::getSharedkey() const {
		return sharedkey;
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
		if(crypto_secretstream_xchacha20poly1305_init_pull(&state, headerBuf, sharedkey.data()) != 0){
			//Incomplete header
			std::cerr << errorPrefix << "Incomplete header" << std::endl;
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
				std::cerr << errorPrefix << "Corrupt chunk @ pos " << index << std::endl;
				cleanup(ciphertext, plaintext, plaintextBuf, ciphertextBuf, headerBuf);
				return Status::FAIL;
			}

			//Check if the stream terminates prematurely
			if(tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !eof){
				//Premature end (end of file reached before the end of the stream)
				std::cerr << errorPrefix << "Premature EOF @ pos " << index << std::endl;
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
		crypto_secretstream_xchacha20poly1305_init_push(&state, headerBuf, sharedkey.data());
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
	bool CryptorEngine::exportPrivkey(const fs::path& dest){
		return CryptorEngine::exportKey(privkey, dest);
	}

	/** Impl of exportPubkey(path). */
	bool CryptorEngine::exportPubkey(const fs::path& dest){
		return CryptorEngine::exportKey(pubkey, dest);
	}
	
	/** Impl of privkeyFingerprint(). */
	std::string CryptorEngine::privkeyFingerprint(){
		return fingerprint(privkey);
	}

	/** Impl of pubkeyFingerprint(). */
	std::string CryptorEngine::pubkeyFingerprint(){
		return fingerprint(pubkey);
	}

	/** Impl of toString(). */
	std::string CryptorEngine::toString(){
		//Create the output string object
		std::string out = "CryptorEngine{";

		//Add the relevant attributes
		out += "cipherSuite=" + CIPHER_ALGO + ", ";
		out += "keyType=" + KEY_TYPE + ", ";
		out += "pubkeyFingerprint=" + pubkeyFingerprint() + ", ";
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


	//Private methods
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

	/** Impl of generateSharedSecret(array, array). */
	std::array<uint8_t, CryptorEngine::SHAREDKEY_SIZE> CryptorEngine::generateSharedSecret(const std::array<uint8_t, CryptorEngine::PRIVKEY_SIZE>& privkey, const std::array<uint8_t, CryptorEngine::PUBKEY_SIZE>& pubkey){
		//This might be called very early so ensure LibSodium is loaded first
		init();

		//Generate the shared secret
		std::array<uint8_t, CryptorEngine::SHAREDKEY_SIZE> sharedkey;
		int sharedKeyRet = crypto_box_beforenm(sharedkey.data(), pubkey.data(), privkey.data());

		//Throw a runtime error if the process failed
		if(sharedKeyRet != 0) throw std::runtime_error("LibSodium could not generate the required shared secret. Return code: " + std::to_string(sharedKeyRet));

		//Return the shared secret
		return sharedkey;
	}

	/** Impl of init(). */
	void CryptorEngine::init(){
		//Initialize LibSodium
		int sodiumInitRetVal = sodium_init();
		if(sodiumInitRetVal == -1) throw std::runtime_error("LibSodium could not be safely initialized. Return code: " + sodiumInitRetVal);
	}

	/** Impl of setupFromPrivkey(array). */
	void CryptorEngine::setupFromPrivkey(const std::array<uint8_t, CryptorEngine::PRIVKEY_SIZE>& key){
		//Initialize LibSodium
		init();

		//Generate the public key from the private key
		privkey = key;
		int pubkeyDerivRet = crypto_scalarmult_base(pubkey.data(), privkey.data());
		if(pubkeyDerivRet != 0) throw std::runtime_error("LibSodium could not generate the required public key. Return code: " + std::to_string(pubkeyDerivRet));

		//Generate the shared secret
		sharedkey = generateSharedSecret(privkey, pubkey);
	}
}

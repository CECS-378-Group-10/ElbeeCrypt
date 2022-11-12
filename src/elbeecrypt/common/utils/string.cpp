#include <algorithm>
#include "elbeecrypt/common/utils/string.hpp"

/** Impl of string.hpp */
namespace elbeecrypt::common::utils::String {
	/** Impl of boolStr(bool). */
	const std::string boolStr(bool boolean){
		return boolean ? "true" : "false";
	}

	/** Impl of contains(string, string, bool). */
	bool contains(const std::string& str, const std::string& target, bool ignoreCase){
		//Clone the input strings
		std::string strCloned(str);
		std::string targetCloned(target);

		//Transform both strings to lowercase if case should be ignored
		if(ignoreCase){
			strCloned = toLowercase(strCloned);
			targetCloned = toLowercase(targetCloned);
		}

		//Perform the check and return the result
		return strCloned.find(targetCloned) != std::string::npos;
	}

	/** Impl of contains(string, string). */
	bool contains(const std::string& str, const std::string& target){
		return contains(str, target, false);
	}

	/** Impl of contains(string, char, bool). */
	bool contains(const std::string& str, const char& target, bool ignoreCase){
		return contains(str, std::string(&target), ignoreCase);
	}

	/** Impl of contains(string, char). */
	bool contains(const std::string& str, const char& target){
		return contains(str, std::string(&target));
	}

	/** Impl of firstIndexOf(string, char). */
	int firstIndexOf(const std::string& str, const char& target){
		//Loop over the string, character by character
		int index = -1;
		for(size_t i = 0; i < str.length(); i++){
			//If the current character equals the target, set the index to be the current i value and break out
			if(str[i] == target){
				index = (int) i;
				break;
			}
		}

		//Return the index
		return index;
	}

	/** Impl of lastIndexOf(string, char). */
	int lastIndexOf(const std::string& str, const char& target){
		//Loop backwards over the string, character by character
		int index = -1;
		for(size_t i = str.length(); i > 0; i--){
			//If the current character equals the target, set the index to be the current i value and break out
			if(str[i] == target){
				index = (int) i;
				break;
			}
		}

		//Return the index
		return index;
	}

	/** Impl of toLowercase(string). */
	const std::string toLowercase(const std::string& str){
		//Clone the input string to maintain immutability, transform it, and return it
		std::string cloned(str);
		std::transform(cloned.begin(), cloned.end(), cloned.begin(), [](char c){
			return static_cast<char>(tolower(c)); //Defined as a lambda to get rid of a compiler warning in MSVC
		});
		return cloned;
	}

	/** Impl of toUppercase(string). */
	const std::string toUppercase(const std::string& str){
		//Clone the input string to maintain immutability, transform it, and return it
		std::string cloned(str);
		std::transform(cloned.begin(), cloned.end(), cloned.begin(), [](char c){
			return static_cast<char>(toupper(c)); //Defined as a lambda to get rid of a compiler warning in MSVC
		});
		return cloned;
	}
}

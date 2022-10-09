#include <regex>
#include "elbeecrypt/common/utils/container.hpp"

/** Impl of container.hpp */
namespace elbeecrypt::common::utils::Container {
	/** Impl of tokenize(string, string). */
	std::vector<std::string> tokenize(const std::string& str, const std::string& regexp){
		//Create a regexp for the given input string
		std::regex regexpr(regexp);

		//Construct a new vector from the regexp using std::sregex_token_iterator
		return std::vector<std::string>(
			std::sregex_token_iterator(str.begin(), str.end(), regexpr, -1),
			std::sregex_token_iterator()
		);
	}
}
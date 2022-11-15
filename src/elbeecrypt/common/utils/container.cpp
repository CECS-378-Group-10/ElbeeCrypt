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

	/** Impl of contains(vector, string, bool). */
	bool contains(const std::vector<std::string>& vec, const std::string& target, bool ignoreCase){
		//Clone the input string for immutability
		std::string targetC = target;

		//Create a lambda to act as the checker
		//https://stackoverflow.com/a/36494706
		auto comparator = std::find_if(vec.begin(), vec.end(), [&targetC, &ignoreCase](const std::string& s){
        	if(s.size() != targetC.size()) return false;
			return std::equal(s.cbegin(), s.cend(), targetC.cbegin(), targetC.cend(), [&ignoreCase](auto c1, auto c2){
				//Return the comparison between each char, transforming to lowercase if case does not matter
				return (
					(ignoreCase ? std::tolower(c1) : c1) ==
					(ignoreCase ? std::tolower(c2) : c2)
				);
			});
		});

		//Check if the target is in the vector and return the result
		return comparator != vec.end();
	}

	/** Impl of contains(vector, string). */
	bool contains(const std::vector<std::string>& vec, const std::string& target){
		return std::find(vec.begin(), vec.end(), target) != vec.end();
	}

	/** Impl of vecStr(vector). */
	std::string vecStr(const std::vector<std::string>& vec){
		//Define the lambda to convert the strings to strings (basically a no-op)
		std::function<std::string(const std::string&)> converter = [](const std::string& str){
			return str;
		};

		//Convert the vector
		return vecStr(vec, converter);
	}

	/** Impl of vecStr(vector). */
	std::string vecPathStr(const std::vector<fs::path>& vec){
		//Define the lambda to convert the paths to strings
		std::function<std::string(const fs::path&)> converter = [](const fs::path& path){
			return path.string();
		};

		//Convert the vector
		return vecStr(vec, converter);
	}
}
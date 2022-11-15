#include <vector>
#include "elbeecrypt/common/utils/stream.hpp"

/** Impl of stream.hpp */
namespace elbeecrypt::common::utils::Stream {
	/** Impl of writeToFile(path, vector). */
	bool writeToFile(const fs::path& target, const std::vector<std::string>& lines){
		//Define the lambda to convert the strings to strings (basically a no-op)
		std::function<std::string(const std::string&)> converter = [](const std::string& str){
			return str;
		};

		//Write to the file and return the result
		return writeToFile(target, lines, converter);
	}

	/** Impl of writeToFile(path, vector). */
	bool writeToFile(const fs::path& target, const std::vector<fs::path>& paths){
		//Define the lambda to convert the paths to strings
		std::function<std::string(const fs::path&)> converter = [](const fs::path& path){
			return path.string();
		};

		//Write to the file and return the result
		return writeToFile(target, paths, converter);
	}

	/** Impl of writeToFile(path, string). */
	bool writeToFile(const fs::path& target, const std::string& line){
		//Create a vector for the singular line and call the relevant function to handle it
		std::vector<std::string> lines = {};
		lines.push_back(line);
		return writeToFile(target, lines);
	}
}
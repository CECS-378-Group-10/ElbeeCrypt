#include "elbeecrypt/common/utils/container.hpp"
#include "elbeecrypt/common/utils/fs.hpp"

/** Impl of fs.hpp */
namespace elbeecrypt::common::utils::FS {
	/** Impl of absLex(path). */
	fs::path absLex(const fs::path& base){
		return fs::absolute(base).lexically_normal();
	}

	/** Impl of appendExt(path, string). */
	fs::path appendExt(const fs::path& path, const std::string& ext){
		//Clone the extension string for immutability
		std::string extC = ext;

		//Check if the ext starts with a dot
		if(extC[0] == '.'){
			//Remove the dot
			extC = extC.substr(1);
  		}

		//Concat the path and the extension
		return fs::path(path.string() + "." + extC);
	}

	/** Impl of appendExt(path, path). */
	fs::path appendExt(const fs::path& path, const fs::path& ext){
		return appendExt(path, ext.string());
	}

	/** Impl of canRead(path). */
	bool canRead(const fs::path& path){
		//Open an ifstream in read mode
		std::ifstream tester(path.string(), std::ios::in);

		//Return the status of is_open
		bool status = tester.is_open();
		tester.close();
		return status;
	}

	/** Impl of canWrite. */
	bool canWrite(const fs::path& path){
		//Open an ofstream in write mode with append to avoid damaging the file
		std::ifstream tester(path.string(), std::ios::app | std::ios::out);

		//Return the status of is_open
		bool status = tester.is_open();
		tester.close();
		return status;
	}

	/** Impl of getParents(vector<path>). */
	std::vector<fs::path> getParents(const std::vector<fs::path>& paths){
		//Create a new vector to store the parent paths
		std::vector<fs::path> parents = {};

		//Loop over the list of paths
		for(fs::path path : paths){
			//Append the parent path of the current path to the vector of parent paths
			parents.push_back(path.parent_path());
		}

		//Remove duplicates and return the list
		elbeecrypt::common::utils::Container::removeDuplicates(parents);
		return parents;
	}

	/** Impl of hasFullRead(perms). */
	bool hasFullRead(const fs::perms& base){
		return
			hasPermission(base, fs::perms::owner_read) && //Octal bit 1
			hasPermission(base, fs::perms::group_read) && //Octal bit 2
			hasPermission(base, fs::perms::others_read); //Octal bit 3
	}

	/** Impl of hasFullWrite(perms). */
	bool hasFullWrite(const fs::perms& base){
		return
			hasPermission(base, fs::perms::owner_write) && //Octal bit 1
			hasPermission(base, fs::perms::group_write) && //Octal bit 2
			hasPermission(base, fs::perms::others_write); //Octal bit 3
	}

	/** Impl of hasPermission(perms, perms). */
	bool hasPermission(const fs::perms& base, const fs::perms& check){
		return (base & check) != fs::perms::none;
	}

	/** Impl of pathDepth(path). */
	uint32_t pathDepth(const fs::path& path){
		//Create a counter
		int depth = 0;

		//Loop while the current path and the parent path are different
		fs::path current = path;
		fs::path parent = current.parent_path();
		while(current != parent){
			//Update the paths
			current = parent;
			parent = current.parent_path();

			//Increment the counter
			depth++;
		}

		//Return the depth
		return depth;
	}

	/** Impl of pathsVecToString(vector<path>). */
	std::string pathsVecToString(const std::vector<fs::path>& paths){
		//Create the output string
		std::string out = "[";

		//Loop over the vector elements
		for(size_t i = 0; i < paths.size(); i++){
			//Append the current path to the output string
			out += paths.at(i).string();

			//Add a separator if the loop isn't at the end
			if(i < paths.size() - 1) out += ", ";
		}

		//Return the output string
		return out + "]";
	}

	/** Impl of removeTrailingExtension(path). */
	fs::path removeTrailingExtension(const fs::path &path){
		//Clone path for immutability
		fs::path pathC = path;

		//Get the path filename and stem
		std::string filename = path.filename().string();
		std::string stem = path.stem().string();

		//Check if the two pieces are unequal. If they are, the path has a filename
		if(filename != stem){
			//Remove the trailing extension via stem
			pathC = pathC.parent_path() / pathC.stem();
		}

		//Return the path, modified or not
		return pathC;
	}
}

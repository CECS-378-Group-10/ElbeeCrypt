#include "elbeecrypt/common/utils/fs.hpp"

/** Impl of fs.hpp */
namespace elbeecrypt::common::utils::FS {
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
	int pathDepth(const fs::path& path){
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
}

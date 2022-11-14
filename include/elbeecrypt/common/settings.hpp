#pragma once

#include <string>
#include <vector>

/**
 * @brief Contains the default settings for the ransomware.
 * 
 * @author CECS-378 Group 10
 * @file settings.hpp
 */
namespace elbeecrypt::common::Settings {
	/** The extension to use for encrypted files. */
	const std::string ENCRYPTED_EXTENSION = "elbeecrypt";

	/** Whether there should be a warning before running. */
	constexpr bool SAFETY_NET = true;

	/** Whether there should be ransom notes dropped in every directory that the ransomware hit. */
	constexpr bool SPAM_RANSOM_NOTES = false;
}
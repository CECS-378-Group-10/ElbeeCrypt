#pragma once

#include <string>

/**
 * @brief Contains the function definitions for the
 * main executable file of the encryption portion of
 * the ransomware.
 * 
 * @author CECS-378 Group 10
 * @file main.hpp
 */
namespace elbeecrypt::cryptor::Main {
	/**
	 * @brief Prompts the user warning them of potential system damage.
	 * Serves as a safety net in case the user accidentally launched the
	 * program on their system.
	 * 
	 * @return Whether the application is allowed to move forward
	 */
	bool safetyNet();

	/**
	 * @brief Runs the encryption payload of the ransomware.
	 */
	void encrypt();
}
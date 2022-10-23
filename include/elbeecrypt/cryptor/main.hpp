#pragma once

#include <string>

namespace elbeecrypt::cryptor::Main {
	/**
	 * @brief Prompts the user warning them of potential system damage.
	 * Serves as a safety net in case the user accidentally launched the
	 * program on their system.
	 * 
	 * @return Whether the application is allowed to move forward
	 */
	bool safetyNet();
}
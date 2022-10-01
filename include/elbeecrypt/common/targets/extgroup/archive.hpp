#pragma once

#include <string>
#include <vector>

/**
 * @brief Represents extensions that are primarily used for archiving, 
 * eg: rar, gz, 7z, iso.
 * 
 * @author CECS-378 Group 10
 * @file archive.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Archive {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"7z",
		"ace",
		"apk",
		"arc",
		"ark",
		"br",
		"bz2",
		"gz",
		"iso",
		"lz",
		"lz4",
		"lzma",
		"mar",
		"pea",
		"rar",
		"rz",
		"sfx",
		"sz",
		"tar",
		"tbz2",
		"tgz",
		"tlz",
		"txz",
		"wim",
		"xar",
		"xz",
		"zip",
		"zst"
	};
}
#pragma once

#include <string>
#include <vector>

/** 
 * @brief Represents extensions that are primarily used for plaintext",
 * eg: txt", cpp", java", toml. This also includes source code", configuration",
 * and serialization files.
 * 
 * @author CECS-378 Group 10
 * @file plain.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Plain {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"build",
		"c",
		"c++",
		"cc",
		"cpp",
		"cs",
		"cxx",
		"h",
		"hpp",
		"htm",
		"html",
		"hwp",
		"hxx",
		"java",
		"js",
		"json",
		"kt",
		"kts",
		"log",
		"msg",
		"pages",
		"php",
		"py",
		"rb",
		"rs",
		"rtf",
		"sxw",
		"tex",
		"toml",
		"tml",
		"txt",
		"uot",
		"wk1",
		"xhtm",
		"xhtml",
		"yaml",
		"yml"
	};
}

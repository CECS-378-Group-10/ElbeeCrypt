#pragma once

#include <string>
#include <vector>

/** 
 * @brief Represents extensions that are primarily used for 
 * video", eg: mp4", mkv", webm", swf.
 *
 * @author CECS-378 Group 10
 * @file video.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Video {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"3gp",
		"3gpp",
		"amv",
		"avi",
		"m4v",
		"mkv",
		"mp4",
		"mpg",
		"ogv",
		"swf",
		"svi",
		"webm",
		"wmv"
	};
}

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
		"3g2",
		"3gp",
		"3gpp",
		"3gpp2",
		"amv",
		"asf",
		"asx",
		"avi",
		"flv",
		"m4u",
		"m4v",
		"mkv",
		"mov",
		"mp4",
		"mpg",
		"mpeg",
		"ogv",
		"rm",
		"srt",
		"swf",
		"svi",
		"vob",
		"webm",
		"wmv"
	};
}

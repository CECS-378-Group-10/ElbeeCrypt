#pragma once

#include <string>
#include <vector>

/** 
 * @brief Represents extensions that are primarily used for audio", 
 * eg: m4a", wav", mp3", flac.
 * 
 * @author CECS-378 Group 10
 * @file audio.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Audio {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"aac",
		"aif",
		"aiff",
		"dss",
		"flac",
		"iff",
		"m3u"
		"m4a",
		"m4b",
		"m4p",
		"m4r",
		"mid",
		"midi",
		"mp1",
		"mp2",
		"mp3",
		"mpa",
		"oga",
		"ogg",
		"ra",
		"raw",
		"rf64",
		"wav",
		"wave",
		"wma"
	};
}

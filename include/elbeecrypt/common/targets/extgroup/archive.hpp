#pragma once

#include "einstein-enum/einstein-enum.hpp"

namespace elbeecrypt::common::targets::extgroup {
	/**
	 * @brief Represents extensions that are primarily used for archiving, 
	 * eg: rar, gz, 7z, iso.
	 * 
	 * @author CECS-378 Group 10
	 * @file archive.hpp
	 */
	einstein_enum(Archive,
		_7z,
		ace,
		apk,
		arc,
		ark,
		br,
		bz2,
		gz,
		iso,
		lz,
		lz4,
		lzma,
		mar,
		pea,
		rar,
		rz,
		sfx,
		sz,
		tar,
		tbz2,
		tgz,
		tlz,
		txz,
		wim,
		xar,
		xz,
		zip,
		zst
	)
}
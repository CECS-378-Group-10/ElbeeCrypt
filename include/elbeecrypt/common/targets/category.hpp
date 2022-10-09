#include "einstein-enum/einstein-enum.hpp"

namespace elbeecrypt::common::targets {
	/**
	 * @brief Represents the category of an extension, eg: document,
	 * video, audio, image, etc.
	 * 
	 * @author CECS-378 Group 10
	 * @file category.hpp
	 */
	einstein_enum(Category,
		ARCHIVE, //Archive files (eg: rar, zip)
		AUDIO, //Audio files (eg: mp3, wav)
		DOCUMENT, //Document files (eg: docx, odt)
		EXECUTABLE, //Executable files (eg: dylib, exe)
		IMAGE, //Image files (eg: jpg, png)
		MISC, //Miscellaneous files (eg: cad, fla)
		PLAIN, //Plaintext files (eg: cpp, hpp)
		UNDEFINED, //Catch-all for unrecognized extensions
		VIDEO //Video files (eg: avi, mp4)
	);
}
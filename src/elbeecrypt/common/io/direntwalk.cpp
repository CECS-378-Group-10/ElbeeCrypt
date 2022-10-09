#include <string>
#include "elbeecrypt/common/io/direntwalk.hpp"

//Windows doesn't come with this header, so it must be provided separately, else use the standard POSIX version
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	#include "windirent/dirent.h"
#else
	#include <dirent.h>
#endif

/** Impl of direntwalk.hpp */
namespace elbeecrypt::common::io::DirentWalk {
	/** Impl of directoryList(path, vector<path>). */
	void directoryList(const fs::path& root, std::vector<fs::path>& paths){
		//Define the lambda to collect the directory listings
		auto consumer = [&paths](const fs::path& path){
			//Add the entry to the input vector
			paths.push_back(path);
		};

		//Call the walk function with the root path and the lambda
		walk(root, consumer);
	}

	/** Impl of walk(path, function<void(path)>). */
	void walk(const fs::path& root, std::function<void(const fs::path&)> consumer){
		//Set up dirent.h
		DIR* dir;
		struct dirent* dirent;

		//Construct the path to use for the dirent operation via janky absolute path creation
		fs::path in = (fs::absolute(root.parent_path()) / root.filename()).lexically_normal();

		//If the directory is readable, perform a dirent list with it
		if((dir = opendir(in.string().c_str())) != NULL){
			//Perform the directory listing with dirent
			size_t index = 0;
			while((dirent = readdir(dir)) != NULL){
				//Skip the first two dirent listings. These include the current directory "." and the parent directory "../"
				if(index < 2){
					index++;
					continue;
				}

				//Construct a path object out of the current dirent listing
				fs::path current = in / dirent->d_name;

				//If the entry is a directory, call the walk method recursively
				if(fs::is_directory(current)){
					walk(current, consumer);
				}
				else {
					//Send the current path to the consumer lambda for further processing
					consumer(current);
				}

				//Increment the counter
				index++;
			}

			//Close out the dirent operation
			closedir(dir);
		}

		//Break out if an error occurs (base case)
		else return;
	}
}
#include <string>
#include "elbeecrypt/common/io/direntwalk.hpp"

//Windows doesn't come with this header, so it must be provided separately, else use the standard POSIX version
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	#include "windirent/dirent.h"

	//Use wdirent on Windows to prevent issues with reading files with non-ASCII filenames
	//Huge thanks to https://stackoverflow.com/a/35065142
	//In short, Windows will use UTF-16 filenames vs UTF-8 on POSIX, so use wdirent instead of dirent
	#define dirent wdirent
	#define DIR WDIR
	#define opendir wopendir
	#define readdir wreaddir
	#define closedir wclosedir
#else
	#include <dirent.h>
#endif

/** Impl of direntwalk.hpp */
namespace elbeecrypt::common::io::DirentWalk {
	/** Impl of directoryList(path, vector<path>). */
	void directoryList(const fs::path& root, std::vector<fs::path>& paths){
		//Define the lambda to collect the directory listings
		auto fileConsumer = [&paths](const fs::path& path){
			//Add the entry to the input vector
			paths.push_back(path);
		};

		//Create a no-op for the folder processor lambda
		auto folderConsumer = [](const fs::path&){
			return true;
		};

		//Call the walk function with the root path and the lambdas
		walk(root, fileConsumer, folderConsumer);
	}

	/** Impl of pwd(). */
	fs::path pwd(){
		fs::path pwd = fs::current_path();
		return (fs::absolute(pwd.parent_path()) / pwd.filename()).lexically_normal();
	}

	/** Impl of walk(path, function<void(path)>, function<bool(path)>). */
	void walk(const fs::path& root, std::function<void(const fs::path&)> fileConsumer, std::function<bool(const fs::path&)> folderConsumer){
		//Set up dirent.h
		DIR* dir;
		struct dirent* dirent;

		//Construct the path to use for the dirent operation via janky absolute path creation
		fs::path in = (fs::absolute(root.parent_path()) / root.filename()).lexically_normal();

		//Get the c-string version of the path as either a wchar_t array (Windows) or a char array (POSIX)
		#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
			dir = opendir(in.wstring().c_str());
		#else
			dir = opendir(in.string().c_str());
		#endif

		//If the directory is readable, perform a dirent list with it
		if(dir != NULL){
			//Perform the directory listing with dirent
			size_t index = 0;
			while((dirent = readdir(dir)) != NULL){
				//Skip the first two dirent listings. These include the current directory "." and the parent directory "../"
				if(index < 2){
					index++;
					continue;
				}

				//Construct a path object out of the current dirent listing
				//Use either a wstring (Windows) or a string (POSIX)
				#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
					fs::path current = in / std::wstring(dirent->d_name);
				#else
					fs::path current = in / std::string(dirent->d_name);
				#endif

				//If the entry is a directory, call the walk method recursively
				if(fs::is_directory(current)){
					//Call the folder consumer and get its output
					bool shouldRecurse = folderConsumer(current);

					//If the lambda returned true, then recursively run the function
					if(shouldRecurse) walk(current, fileConsumer, folderConsumer);
				}
				else {
					//Send the current path to the file consumer lambda for further processing
					fileConsumer(current);
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
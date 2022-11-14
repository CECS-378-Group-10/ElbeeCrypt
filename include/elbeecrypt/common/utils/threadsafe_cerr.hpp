#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

namespace elbeecrypt::common::utils {
	/** 
	 * Thread safe cerr class
	 * Example of use:
	 * 	Cerr{} << "Hello world!" << std::endl;
	 * https://stackoverflow.com/a/41602842
	 */
	class Cerr: public std::ostringstream {
		public:
			Cerr() = default;

			~Cerr(){
				std::lock_guard<std::mutex> guard(_mutexPrint);
				std::cerr << this->str();
			}

		private:
			static std::mutex _mutexPrint;
	};

	std::mutex Cerr::_mutexPrint{};
}
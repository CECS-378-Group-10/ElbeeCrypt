#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

namespace elbeecrypt::common::utils {
	/** 
	 * Thread safe cout class
	 * Example of use:
	 * 	Cout{} << "Hello world!" << std::endl;
	 * https://stackoverflow.com/a/41602842
	 */
	class Cout: public std::ostringstream {
		public:
			Cout() = default;

			~Cout(){
				std::lock_guard<std::mutex> guard(_mutexPrint);
				std::cout << this->str();
			}

		private:
			static std::mutex _mutexPrint;
	};

	std::mutex Cout::_mutexPrint{};
}
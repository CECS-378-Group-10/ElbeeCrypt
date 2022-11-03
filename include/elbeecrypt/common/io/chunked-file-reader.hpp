#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace elbeecrypt::common::io {
	/**
	 * @brief Reads a file in chunks of x bytes large. Allows for the 
	 * processing of very large files without having to load the entire
	 * file into memory. This class uses C++'s native `fstream` class
	 * to do this.
	 * 
	 * @author CECS-378 Group 10
	 * @file chunked-file-reader.hpp
	 */
	class ChunkedFileReader {
		//Fields
		private:
			/** The source path from which the file was read. */
			const fs::path sourcePath;

			/** The size of each chunk in bytes. */
			const uint64_t _chunkSize;

			/** The total number of chunks in the file. */
			size_t chunkCount;

			/** The total size of the file in bytes. */
			uint64_t fileSize;

			/** The input file stream for the current file. */
			std::ifstream fileStream;


		//Constructors & Destructor
		public:
			/**
			 * @brief Constructs a new Chunked File Reader object.
			 * 
			 * @param source The source file to read from
			 * @param chunkSize The size of each chunk in bytes
			 */
			ChunkedFileReader(fs::path source, uint64_t chunkSize);

			/**
			 * @brief Destroys the current Chunked File Reader object.
			 */
			~ChunkedFileReader();


		//Getters
		public:
			/**
			 * @brief Gets the total number of chunks in the file.
			 * 
			 * @return The total number of chunks in the file
			 */
			size_t getChunkCount();

			/**
			 * @brief Gets the size of each chunk in bytes. Keep in mind
			 * that this method only returns the *maximum size* of a chunk.
			 * Not all chunks may be this size. For example, the last chunk
			 * in the file may be smaller if the upper bound of the chunk
			 * goes beyond the end of the file. Obviously, if this is the
			 * case, the upper bound should be no bigger than the end of 
			 * the file to avoid overflow errors.
			 * 
			 * @return The size of each chunk in bytes
			 */
			uint64_t getChunkSize();

			/**
			 * @brief Gets the total size of the file in bytes.
			 * 
			 * @return The total size of the file in bytes
			 */
			uint64_t getFileSize();

			/**
			 * @brief Gets the source path from which the file was read.
			 * 
			 * @return The source path from which the file was read from
			 */
			fs::path getSourcePath();


		//Methods
		public:
			/**
			 * @brief Reads a byte chunk at a given index. The size of this
			 * chunk can be found out by calling `chunkSizeAt()`, as the size
			 * of a given chunk is non-trivial. Do keep in mind that this
			 * function allocates a byte array on the heap, leaving its 
			 * management to the user. Be sure to delete this allocated byte
			 * array once you are done via `delete[] myByteArray`. Throws
			 * an out of range exception if the index is beyond the number
			 * of chunks.
			 * 
			 * @param index The position of the chunk to fetch
			 * @return The byte chunk at the given index
			 */
			uint8_t* chunkAt(const size_t& index);

			/**
			 * @brief Gets the size of the chunk at index i. This
			 * method will return 0 if the chunk index is invalid.
			 * Big thanks to freezing_ on Stack OverFlow for the
			 * solution involving the modulo operation. The relevant
			 * thread can be found here: https://stackoverflow.com/a/68502252.
			 * Throws an out of range exception if the index is 
			 * beyond the number of chunks.
			 * 
			 * @param index The index to get the chunk size for
			 * @return The size of the chunk at index i
			 */
			uint64_t chunkSizeAt(const size_t& index);

			/**
			 * @brief Returns whether the last chunk is sized differently
			 * than what `chunkSize` is. This occurs if the file size and
			 * the chunk size do not divide evenly. This results in a final
			 * chunk that is smaller than the others. To get the size of
			 * this final chunk, run `chunkSizeAt(getChunkCount() - 1)`.
			 * 
			 * @return Whether the last chunk is unevenly sized
			 */
			bool hasUnevenLastChunk();

			/**
			 * @brief Gets the size of the last chunk in the file. If
			 * `hasUnevenLastChunk()` is true, this value will differ
			 * from `getChunkSize()`. This function is an alias for 
			 * `chunkSizeAt(getChunkCount() - 1)`, and provides a 
			 * more straightforward way of getting this info.
			 * 
			 * @return uint64_t The size of the last chunk in the file
			 */
			uint64_t lastChunkSize();

			/**
			 * @brief Returns the string representation of this object.
			 * 
			 * @return The string representation of this object
			 */
			std::string toString();

		
		//Operator overloads
		public:
			/**
			 * @brief Reads a byte chunk at a given index. The size of this
			 * chunk can be found out by calling `chunkSizeAt()`, as the size
			 * of a given chunk is non-trivial. This is an alias of the `chunkAt()`
			 * function, but accessible via the array subscript operator. 
			 * Bounds checks are still enforced unlike many implementations
			 * of this operator in STL classes. Do keep in mind that this
			 * function allocates a byte array on the heap, leaving its 
			 * management to the user. Be sure to delete this allocated byte
			 * array once you are done via `delete[] myByteArray`. Throws
			 * an out of range exception if the index is beyond the number
			 * of chunks.
			 * 
			 * @param index The position of the chunk to fetch
			 * @return The byte chunk at the given index
			 */
			uint8_t* operator[](const size_t& index);

			/**
			 * @brief Returns the string representation of this object. 
			 * Also allows for implicit string casting. 
			 * 
			 * @return The string representation of this object
			 */
			operator std::string();

			friend std::ostream& operator<<(std::ostream& os, const ChunkedFileReader& cfr);


		//Private methods
		private:
			/**
			 * @brief Calculates the total number of chunks in the file.
			 * 
			 * @return The total number of chunks in the file
			 */
			size_t calculateChunkCount();

			/**
			 * @brief Calculates the total size of the file in bytes.
			 * 
			 * @return The total size of the file in bytes
			 */
			uint64_t calculateFileSize();

			/**
			 * @brief Guard function that performs a bounds check for a
			 * given index against the number of chunks.
			 * 
			 * @param index The index to bounds check
			 */
			void chunkBoundsCheck(size_t index);

			/**
			 * @brief Converts an arbitrary datatype to a string using
			 * a string stream. Adapted from https://stackoverflow.com/a/7348542
			 * 
			 * @tparam T The datatype of the item to convert
			 * @param target The item to convert
			 * @return The resultant string
			 */
			template<typename T>
			static std::string ssString(const T& target){
				std::ostringstream out;
				out << target;
				return out.str();
			}
	};
}

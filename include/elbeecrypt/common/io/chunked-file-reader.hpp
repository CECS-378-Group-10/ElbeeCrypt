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
		//Constants
		private:
			/** The default size of the post-data footer in each chunk in bytes. */
			const static uint DEFAULT_FOOTER_SIZE = 0;

			/** The default size of the pre-data header in each chunk in bytes. */
			const static uint DEFAULT_HEADER_SIZE = 0;


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

			/** The size of the post-data footer in each chunk in bytes. This will be 0 bytes in size by default. */
			uint footerSize = DEFAULT_FOOTER_SIZE;

			/** The size of the pre-data header in each chunk in bytes. This will be 0 bytes in size by default. */
			uint headerSize = DEFAULT_HEADER_SIZE;


		//Constructors & Destructor
		public:
			/**
			 * @brief Constructs a new Chunked File Reader object. Throws a
			 * `runtime_error` if the file at the source path cannot be read.
			 * 
			 * @param source The source file to read from
			 * @param chunkSize The size of each chunk in bytes
			 * @throws std::runtime_error If the file at the source path cannot be read
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
			 * @brief Gets the size of the data portion of each chunk in
			 * bytes. This value is acquired via subtracting the header
			 * and footer size from the total chunk size. Keep in mind
			 * that this method only returns the *maximum size* of a chunk's
			 * data portion. Not all chunk data portions may be this size. 
			 * For example, the last chunk in the file may be smaller if 
			 * the upper bound of the chunk goes beyond the end of the file. 
			 * Obviously, if this is the case, the upper bound should be no 
			 * bigger than the end of the file to avoid overflow errors.
			 * 
			 * @return The size of each chunk's data portion in bytes
			 */
			uint64_t getChunkDataSize();

			/**
			 * @brief Gets the total size of the file in bytes.
			 * 
			 * @return The total size of the file in bytes
			 */
			uint64_t getFileSize();

			/**
			 * @brief Gets the size the post-data footer in each chunk in bytes. 
			 * This will be 0 bytes in size by default. Every single chunk in the 
			 * file will have this size allocation at the end past the data section.
			 * The same thing holds for the last chunk in the file.
			 * 
			 * @return The size of the post-data footer in each chunk in bytes 
			 */
			uint getFooterSize();

			/**
			 * @brief Gets the size the pre-data header in each chunk in bytes. 
			 * This will be 0 bytes in size by default. Every single chunk in the 
			 * file will have this size allocation at the beginning before the data 
			 * section. The same thing holds for the last chunk in the file.
			 * 
			 * @return The size of the pre-data header in each chunk in bytes 
			 */
			uint getHeaderSize();

			/**
			 * @brief Gets the source path from which the file was read.
			 * 
			 * @return The source path from which the file was read from
			 */
			fs::path getSourcePath();

		
		//Setters
		public:
			/**
			 * @brief Sets the size of the post-data footer in each chunk in 
			 * bytes. Do keep in  mind that this value and the header value 
			 * must not exceed the size of the chunk, as these values subtract 
			 * space away from the chunk's data portion.
			 * 
			 * @param newFooterSize The new size of the post-data footer in each chunk in bytes
			 * @return The modified `ChunkedFileReader` object (allows call chaining)
			 */
			ChunkedFileReader& setFooterSize(const uint& newFooterSize);

			/**
			 * @brief Sets the size of the pre-data header in each chunk in 
			 * bytes. Do keep in  mind that this value and the footer value 
			 * must not exceed the size of the chunk, as these values subtract 
			 * space away from the chunk's data portion.
			 * 
			 * @param newHeaderSize The new size of the pre-data header in each chunk in bytes
			 * @return The modified `ChunkedFileReader` object (allows call chaining)
			 */
			ChunkedFileReader& setHeaderSize(const uint& newHeaderSize);


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
			 * @throws std::out_of_range If the index is beyond the number of chunks
			 */
			uint8_t* chunkAt(const size_t& index);

			/**
			 * @brief Reads a byte chunk's data field at a given index. The 
			 * size of this chunk can be found out by calling `chunkDataSizeAt()`, 
			 * as the size of a given data chunk is non-trivial. Do keep in 
			 * mind that this function allocates a byte array on the heap, 
			 * leaving its management to the user. Be sure to delete this 
			 * allocated byte array once you are done via `delete[] myByteArray`. 
			 * Throws an out of range exception if the index is beyond the 
			 * number of chunks.
			 * 
			 * @param index The position of the chunk to fetch
			 * @return The byte chunk at the given index
			 * @throws std::out_of_range If the index is beyond the number of chunks
			 */
			uint8_t* chunkDataAt(const size_t& index);

			/**
			 * @brief Gets the size of the chunk at index i, minus 
			 * the sizes of the header and footer Throws an out of 
			 * range exception if the index is beyond the number of 
			 * chunks.
			 * 
			 * @param index The index to get the chunk size for
			 * @return The size of the chunk at index i, not including the header and footer sizes
			 * @throws std::out_of_range If the index is beyond the number of chunks
			 */
			uint64_t chunkDataSizeAt(const size_t& index);

			/**
			 * @brief Gets the size of the chunk at index i. Throws 
			 * an out of range exception if the index is beyond the 
			 * number of chunks. Big thanks to freezing_ on Stack 
			 * OverFlow for the solution involving the modulo operation. 
			 * The relevant thread can be found here: 
			 * https://stackoverflow.com/a/68502252.
			 * 
			 * @param index The index to get the chunk size for
			 * @return The size of the chunk at index i
			 * @throws std::out_of_range If the index is beyond the number of chunks
			 */
			uint64_t chunkSizeAt(const size_t& index);

			/**
			 * @brief Returns the end index of the data in the chunk. 
			 * This index is exclusive, ie: subtract 1 from this value
			 * to get the true index value.
			 * 
			 * @param index The index of the relevant chunk
			 * @return The exclusive end index of the chunk data
			 */
			uint64_t dataEnd(const size_t& index);

			/**
			 * @brief Returns the start index of the data in the chunk. 
			 * This index is inclusive.
			 * 
			 * @param index The index of the relevant chunk
			 * @return The inclusive start index of the chunk data
			 */
			uint64_t dataStart(const size_t& index);

			/**
			 * @brief Returns the end index of the footer in the chunk. 
			 * This index is exclusive, ie: subtract 1 from this value
			 * to get the true index value.
			 * 
			 * @param index The index of the relevant chunk
			 * @return The exclusive end index of the chunk footer
			 */
			uint64_t footerEnd(const size_t& index);

			/**
			 * @brief Returns the start index of the footer in the chunk. 
			 * This index is inclusive.
			 * 
			 * @param index The index of the relevant chunk
			 * @return The inclusive start index of the chunk footer
			 */
			uint64_t footerStart(const size_t& index);

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
			 * @brief Returns the end index of the header in the chunk. 
			 * This index is exclusive, ie: subtract 1 from this value
			 * to get the true index value.
			 * 
			 * @param index The index of the relevant chunk
			 * @return The exclusive end index of the header data
			 */
			uint64_t headerEnd(const size_t& index);

			/**
			 * @brief Returns the start index of the header in the chunk. 
			 * This index is inclusive.
			 * 
			 * @param index The index of the relevant chunk
			 * @return The inclusive start index of the chunk header
			 */
			uint64_t headerStart(const size_t& index);

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
			 * @brief Gets the size of the last chunk in the file, minus
			 * the sizes of the header and footer. If `hasUnevenLastChunk()` 
			 * is true, this value will differ from `getChunkDataSize()`. 
			 * This function is an alias for `chunkDataSizeAt(getChunkCount() - 1)`, 
			 * and provides a more straightforward way of getting this info.
			 * 
			 * @return uint64_t The size of the last chunk in the file, not including the header and footer sizes
			 */
			uint64_t lastChunkDataSize();

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
			 * of chunks. The subscript operator will return the contents
			 * of the whole chunk, including the header and footer. If this
			 * data isn't wanted, use `chunkDataAt()` instead.
			 * 
			 * @param index The position of the chunk to fetch
			 * @return The byte chunk at the given index
			 * @throws std::out_of_range If the index is beyond the number of chunks
			 */
			uint8_t* operator[](const size_t& index);

			/**
			 * @brief Returns the string representation of this object. 
			 * Also allows for implicit string casting. 
			 * 
			 * @return The string representation of this object
			 */
			operator std::string();

			/**
			 * @brief Allows for the writing of this object's string 
			 * representation to a specified output stream.
			 * 
			 * @param os The output stream to write to
			 * @param cfr The class instance to write
			 * @return The modified ostream object
			 */
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
			 * @throws std::out_of_range If the index is beyond the number of chunks
			 */
			void chunkBoundsCheck(const size_t& index);

			/**
			 * @brief Guard function that ensures that the total pre and
			 * post-padding around the data is less than the size of the 
			 * chunk. Essentially, there should be at least one byte left
			 * for data in a chunk that isn't padding.
			 * 
			 * @param headerSize
			 * @throws std::invalid_argument If the sum of the two paddings is greater than or equal to the chunk size
			 */
			void paddingCheck(const uint& headerSize, const uint& footerSize);

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

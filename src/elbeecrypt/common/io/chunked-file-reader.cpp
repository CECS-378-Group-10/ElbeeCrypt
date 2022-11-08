#include <cstring>
#include "elbeecrypt/common/io/chunked-file-reader.hpp"

namespace elbeecrypt::common::io {
	//Constructors & Destructor
	/** Impl of ChunkedFileReader(path, uint64_t). */
	ChunkedFileReader::ChunkedFileReader(fs::path source, uint64_t chunkSize) : sourcePath(source), _chunkSize(chunkSize){
		//Initialize the ifstream object
		fileStream = std::ifstream(source.string(), std::ios::binary | std::ios::in);
		fileStream.unsetf(std::ios::skipws);

		//If an issue occurred while reading the file, throw an exception
		if(!fileStream.good()){
			//Determine what went wrong based on the state bit (https://cplusplus.com/reference/ios/ios_base/iostate/)
			std::string errorName;
			switch(fileStream.rdstate()){
				case std::ios::badbit: //Read error during an I/O operation
					errorName = "READ ERROR";
					break;
				case std::ios::eofbit: //Nothing to read in
					errorName = "EMPTY FILE";
					break;
				case std::ios::failbit: //Logical error during an I/O operation
					errorName = "LOGICAL ERROR";
					break;
				default: //Default state
					errorName = "<UNKNOWN>";
					break;
			}

			//Construct and throw the exception
			throw std::runtime_error("Encountered exception while reading file at path '" + source.string() + "': " + errorName);
		}

		//Calculate the other attributes from the ifstream and chunk size
		fileSize = calculateFileSize();
		chunkCount = calculateChunkCount();
	}

	/** Impl of ~ChunkedFileReader(). */
	ChunkedFileReader::~ChunkedFileReader(){
		//Close the ifstream object
		fileStream.close();
	}


	//Getters
	/** Impl of getChunkCount(). */
	size_t ChunkedFileReader::getChunkCount(){
		return chunkCount;
	}

	/** Impl of getChunkSize(). */
	uint64_t ChunkedFileReader::getChunkSize(){
		return _chunkSize;
	}

	/** Impl of getChunkDataSize(). */
	uint64_t ChunkedFileReader::getChunkDataSize(){
		return _chunkSize - headerSize - footerSize;
	}

	/** Impl of getFileSize(). */
	uint64_t ChunkedFileReader::getFileSize(){
		return fileSize;
	}

	/** Impl of getFooterSize(). */
	uint32_t ChunkedFileReader::getFooterSize(){
		return footerSize;
	}

	/** Impl of getHeaderSize(). */
	uint32_t ChunkedFileReader::getHeaderSize(){
		return headerSize;
	}

	/** Impl of getSourcePath(). */
	fs::path ChunkedFileReader::getSourcePath(){
		return sourcePath;
	}


	//Setters
	/** Impl of setFooterSize(uint32_t). */
	ChunkedFileReader& ChunkedFileReader::setFooterSize(const uint32_t& newFooterSize){
		//Do a check to ensure the padding size doesn't exceed the size of the chunk
		paddingCheck(headerSize, newFooterSize);

		//Set the new footer size and recalculate the number of chunks
		this->footerSize = newFooterSize;
		this->chunkCount = calculateChunkCount();

		//Return the modified object for further processing
		return *this;
	}

	/** Impl of setHeaderSize(uint32_t). */
	ChunkedFileReader& ChunkedFileReader::setHeaderSize(const uint32_t& newHeaderSize){
		//Do a check to ensure the padding size doesn't exceed the size of the chunk
		paddingCheck(newHeaderSize, footerSize);

		//Set the new header size and recalculate the number of chunks
		this->headerSize = newHeaderSize;
		this->chunkCount = calculateChunkCount();

		//Return the modified object for further processing
		return *this;
	}


	//Methods
	/** Impl of chunkAt(size_t). */
	uint8_t* ChunkedFileReader::chunkAt(const size_t& index){
		//Get the contents of the data portion of the current chunk
		uint8_t* chunkData = chunkDataAt(index);

		//Get the size of the chunk at the current position
		size_t chunkSize = chunkSizeAt(index);

		//Allocate the necessary byte array for the output
		uint8_t* bytes = new uint8_t[chunkSize]; //Be sure to toss this with `delete[] bytes` once finished! Memory leaks are VERY BAD
		memset(bytes, 0, sizeof(uint8_t) * chunkSize); //dest, data, sizeof(dest)

		//Copy the chunk data into the output array starting at the first available byte after the header
		//Saved a for loop thanks to: https://stackoverflow.com/a/1163943
		memcpy(&bytes[headerSize], &chunkData[0], chunkDataSizeAt(index) * sizeof(uint8_t)); //dest[pos], src[pos], sizeof(src)

		//Deallocate the memory held by the chunk data array and return the output array
		delete[] chunkData;
		return bytes;
	}

	/** Impl of chunkDataAt(size_t). */
	uint8_t* ChunkedFileReader::chunkDataAt(const size_t& index){
		//Ensure that the position is in bounds before continuing
		chunkBoundsCheck(index);

		//Calculate the start position and chunk size for the current index
		const uint64_t start = index * getChunkDataSize();
		const uint64_t size = chunkDataSizeAt(index);

		//Allocate the necessary byte array for the output
		uint8_t* bytes = new uint8_t[size]; //Be sure to toss this with `delete[] bytes` once finished! Memory leaks are VERY BAD

		//Seek to the start and read from the file in one fell swoop
		fileStream.seekg(start, std::ios::beg);
		fileStream.read(reinterpret_cast<char*>(bytes), size); //https://stackoverflow.com/a/39402925

		//Return the byte array
		return bytes;
	}

	/** Impl of chunkDataSizeAt(size_t). */
	uint64_t ChunkedFileReader::chunkDataSizeAt(const size_t& index){
		//Ensure that the position is in bounds before continuing
		chunkBoundsCheck(index);

		//Derive the size of the data chunk
		size_t dataChunkSize = getChunkDataSize();

		//Calculate the start and end position of the pointer in the file
		uint64_t startPos = dataChunkSize * index;
		uint64_t endPos = startPos + dataChunkSize;

		//If the end position is beyond the bounds of the file, simply return the remainder of the file size divided by the chunk size
		if(endPos > fileSize){
			return fileSize % dataChunkSize;
		}
		else {
			//Return the delta between the start and end position
			return endPos - startPos;
		}
	}

	/** Impl of chunkSizeAt(size_t). */
	uint64_t ChunkedFileReader::chunkSizeAt(const size_t& index){
		//The number of chunks is dependent on the size of a data chunk, so get the data chunk size and add back the header and footer
		return headerSize + chunkDataSizeAt(index) + footerSize;
	}

	/** Impl of dataStart(size_t). */
	uint64_t ChunkedFileReader::dataEnd(const size_t& index){
		return chunkSizeAt(index) - footerSize;
	}

	/** Impl of dataStart(size_t). */
	uint64_t ChunkedFileReader::dataStart(const size_t&){
		return headerSize;
	}

	/** Impl of footerEnd(size_t). */
	uint64_t ChunkedFileReader::footerEnd(const size_t& index){
		return chunkSizeAt(index);
	}

	/** Impl of footerStart(size_t). */
	uint64_t ChunkedFileReader::footerStart(const size_t& index){
		return chunkSizeAt(index) - footerSize;
	}

	/** Impl of hasUnevenLastChunk(). */
	bool ChunkedFileReader::hasUnevenLastChunk(){
		//If there is only 1 chunk in the file, then the last chunk is not uneven
		if(chunkCount < 2) return false;

		//Compare the size of the second to last chunk with the size of the last chunk
		//If they are unequal, the last chunk is uneven
		uint64_t secondLastSize = chunkDataSizeAt(chunkCount - 2);
		uint64_t lastSize = chunkDataSizeAt(chunkCount - 1);
		return secondLastSize != lastSize;
		//return fileSize % _chunkSize > 0;
	}

	/** Impl of headerEnd(size_t). */
	uint64_t ChunkedFileReader::headerEnd(const size_t&){
		return headerSize;
	}

	/** Impl of headerStart(size_t). */
	uint64_t ChunkedFileReader::headerStart(const size_t&){
		return 0;
	}

	/* Impl of lastChunkSize(). */
	uint64_t ChunkedFileReader::lastChunkSize(){
		return chunkSizeAt(chunkCount - 1);
	}

	/* Impl of lastChunkDataSize(). */
	uint64_t ChunkedFileReader::lastChunkDataSize(){
		return chunkDataSizeAt(chunkCount - 1);
	}

	/** Impl of toString(). */
	std::string ChunkedFileReader::toString(){
		//Create the output string object
		std::string out = "ChunkedFileReader{";

		//Add the relevant attributes
		out += "chunkCount=" + ssString(chunkCount) + ", ";
		out += "chunkSize=" + ssString(_chunkSize) + ", ";
		out += "headerSize=" + ssString(headerSize) + ", ";
		out += "footerSize=" + ssString(footerSize) + ", ";
		out += "fileSize=" + ssString(fileSize) + ", ";
		out += std::string("hasUnevenLastChunk=") + (hasUnevenLastChunk() ? "true" : "false") + ", "; //Need the constructor, as we are concatting a ternary
		out += "lastChunkSize=" + ssString(lastChunkSize()) + ", ";
		out += "sourcePath=" + sourcePath.string();

		//Close off the output string and return it
		return out += "}";
	}


	//Operator overloads
	/** Impl of operator[](). */
	uint8_t* ChunkedFileReader::operator[](const size_t& index){
		return chunkAt(index);
	}

	/** Impl of operator string(). */
	ChunkedFileReader::operator std::string(){
		return toString();
	}

	/** Impl of operator<<(ostream, ChunkedFileReader). */
	std::ostream& operator<<(std::ostream& os, const ChunkedFileReader& cfr){
		//Perform a const cast hack so toString() can be called in this friend function
		//Thanks to: https://www.cppstories.com/2020/11/share-code-const-nonconst.html/#const_cast-from-non-const-function)
		const std::string str = const_cast<ChunkedFileReader&>(cfr).toString();

		//Write the string representation to the ostream object and return it
		os << str;
		return os;
	}


	//Private methods
	/** Impl of calculateChunkCount(). */
	size_t ChunkedFileReader::calculateChunkCount(){
		//Derive the size of the data chunk
		size_t dataChunkSize = getChunkDataSize();

		//Calculate the number of chunks by dividing the filesize by the chunk size
		size_t _chunkCount = fileSize / dataChunkSize;
		size_t overflow = fileSize % dataChunkSize;

		//If there is overflow, increment the chunk count by 1 to account for this last chunk (will be sized differently than the others)
		return overflow > 0 ? _chunkCount + 1 : _chunkCount;
	}

	/** Impl of calculateFileSize(). */
	uint64_t ChunkedFileReader::calculateFileSize(){
		//Seek from beginning to end and return the resultant size
		fileStream.seekg(0, std::ios::end);
		uint64_t _fileSize = fileStream.tellg();
		fileStream.seekg(0, std::ios::beg);
		return _fileSize;
	}

	/* Impl of chunkBoundsCheck(size_t). */
	void ChunkedFileReader::chunkBoundsCheck(const size_t& index){
		if(index >= chunkCount) throw std::out_of_range(
			"Chunk position out of bounds; position: " + ssString(index) + 
			", range: [" + ssString(0) + ", " + ssString(chunkCount - 1) + "]"
		);
	}

	/** Impl of paddingCheck(uint32_t, uint32_t). */
	void ChunkedFileReader::paddingCheck(const uint32_t& hSize, const uint32_t& fSize){
		if(hSize + fSize >= _chunkSize) throw std::invalid_argument(
			"Header and footer sizes must not exceed the maximum size of a chunk; header size: " +
			ssString(hSize) + " bytes, footer size: " + ssString(fSize) + 
			" bytes. Maximum chunk size: " + ssString(_chunkSize) + " bytes"
		);
	}
}

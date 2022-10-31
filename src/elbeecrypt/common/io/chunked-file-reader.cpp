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

	/** Impl of getFileSize(). */
	uint64_t ChunkedFileReader::getFileSize(){
		return fileSize;
	}

	/** Impl of getSourcePath(). */
	fs::path ChunkedFileReader::getSourcePath(){
		return sourcePath;
	}


	//Methods
	/** Impl of chunkAt(size_t). */
	uint8_t* ChunkedFileReader::chunkAt(const size_t& index){
		//Ensure that the position is in bounds before continuing
		chunkBoundsCheck(index);

		//Calculate the start position and chunk size for the current index
		const uint64_t start = index * _chunkSize;
		const uint64_t size = chunkSizeAt(index);

		//Allocate the necessary byte array for the output
		uint8_t* bytes = new uint8_t[size]; //Be sure to toss this with `delete[] bytes` once finished! Memory leaks are VERY BAD

		//Seek to the start and read from the file in one fell swoop
		fileStream.seekg(start, std::ios::beg);
		fileStream.read(reinterpret_cast<char*>(bytes), size); //https://stackoverflow.com/a/39402925

		//Return the byte array
		return bytes;
	}

	/** Impl of chunkSizeAt(size_t). */
	uint64_t ChunkedFileReader::chunkSizeAt(const size_t& index){
		//Ensure that the position is in bounds before continuing
		chunkBoundsCheck(index);

		//Calculate the start and end position of the pointer in the file
		uint64_t startPos = _chunkSize * index;
		uint64_t endPos = startPos + _chunkSize;

		//If the end position is beyond the bounds of the file, simply return the remainder of the file size divided by the chunk size
		if(endPos > fileSize){
			return fileSize % _chunkSize;
		}
		else {
			//Return the delta between the start and end position
			return endPos - startPos;
		}
	}

	/** Impl of hasUnevenLastChunk(). */
	bool ChunkedFileReader::hasUnevenLastChunk(){
		return fileSize % _chunkSize > 0;
	}

	/* Impl of lastChunkSize(). */
	uint64_t ChunkedFileReader::lastChunkSize(){
		return chunkSizeAt(chunkCount - 1);
	}

	/** Impl of toString(). */
	std::string ChunkedFileReader::toString(){
		//Create the output string object
		std::string out = "ChunkedFileReader{";

		//Add the relevant attributes
		out += "chunkCount=" + ssString(chunkCount) + ", ";
		out += "chunkSize=" + ssString(_chunkSize) + ", ";
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
		//Calculate the number of chunks by dividing the filesize by the chunk size
		size_t _chunkCount = fileSize / _chunkSize;
		size_t overflow = fileSize % _chunkSize;

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
	void ChunkedFileReader::chunkBoundsCheck(size_t index){
		if(index >= chunkCount) throw std::out_of_range("Chunk position out of bounds; position: " + ssString(index) + ", range: [" + ssString(0) + ", " + ssString(chunkCount - 1) + "]");
	}
}
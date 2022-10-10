#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief A fork of Smart Enum with an emphasis on containment
 * of member functions and parameters. The majority of the 
 * functionality of Smart Enum has been retained, but features
 * such as enum-only generation have been removed. The template
 * this library uses is based on a StackOverflow response by 
 * Konchog which can be found here: https://stackoverflow.com/a/55158385.
 * The original Smart Enum library can be found here: https://github.com/therocode/smartenum.
 * 
 * @file einstein-enum.hpp
 */
namespace einstein_enum {
	inline std::string trimWhitespace(const std::string& str){
		//Clone the input string to maintain immutability
		std::string out(str);

		//Trim trailing whitespace
		size_t endPos = out.find_last_not_of(" \t");
		if(std::string::npos != endPos){
			out = out.substr(0, endPos + 1);
		}

		//Trim leading spaces
		size_t startPos = out.find_first_not_of(" \t");
		if(std::string::npos != startPos){
			out = out.substr(startPos);
		}

		//Allows enum names starting with numbers. Simply prefix the enum with a _ and its string representation will exclude the leading _
		if(out.at(0) == '_' && isdigit(out.at(1))){ //Implements the regex "^_[0-9]{1}" (much faster)
			out = out.substr(1);
		}

		return out;
	}

	inline std::string extractEntryAndTrim(std::string& valuesString){
		std::string result;
		size_t nextCommaPos = valuesString.find(',');

		if(nextCommaPos != std::string::npos){
			std::string segment = valuesString.substr(0, nextCommaPos);
			valuesString.erase(0, nextCommaPos + 1);
			result = trimWhitespace(segment);
		}
		else {
			result = trimWhitespace(valuesString);
			valuesString = "";
		}

		return result;
	}

	inline void parseEnumData(const std::string& rawEnumData, std::function<void(const std::string&, const int32_t&)> consumer){
		//Clone the input string to maintain immutability
		std::string processed(rawEnumData);

		//Loop over the enum values while there is still more to process
		int32_t currentEnumValue = 0;
		while(processed != ""){
			//Get the current enum key
			std::string currentEnumEntry = extractEntryAndTrim(processed);

			//Separate the key from the value at the equals sign if it exists
			size_t equalSignPos = currentEnumEntry.find('=');
			if(equalSignPos != std::string::npos){
				//Get the right-hand side of the assignment, set the enum value to be that value, and erase the right hand side from the entry
				std::string rightHandSide = currentEnumEntry.substr(equalSignPos + 1);
				currentEnumValue = std::stoi(rightHandSide);
				currentEnumEntry.erase(equalSignPos);
			}
			
			//Trim any whitespace in the enum entry
			currentEnumEntry = trimWhitespace(currentEnumEntry);
			
			//Pass the entry and value to the consumer for further processing
			consumer(currentEnumEntry, currentEnumValue);
			
			//Increment the enum value
			currentEnumValue++;
		}
	}

	inline std::unordered_map<int32_t, std::string> makeEnumNamesMap(const std::string& enumValuesString){
		//Create the name map
		std::unordered_map<int32_t, std::string> namesMap;

		//Define the lambda that will handle the data from the enum data parser
		auto mapPopulator = [&namesMap](const std::string& key, const int32_t& value){
			namesMap[value] = key;
		};

		//Call the enum data parser with the raw enum string and the lambda
		parseEnumData(enumValuesString, mapPopulator);

		//Return the filled map
		return namesMap;
	}

	inline std::unordered_map<std::string, int32_t> makeEnumValuesMap(const std::string& enumValuesString){
		//Create the value map
		std::unordered_map<std::string, int32_t> valuesMap;

		//Define the lambda that will handle the data from the enum data parser
		auto mapPopulator = [&valuesMap](const std::string& key, const int32_t& value){
			valuesMap[key] = value;
		};

		//Call the enum data parser with the raw enum string and the lambda
		parseEnumData(enumValuesString, mapPopulator);

		//Return the filled map
		return valuesMap;
	}

	template<typename Type, typename Type_Val> //Two types are declared: the outer type (the outer class) and the inner value type (the enum itself)
	std::vector<Type> makeEnumItemsVec(const std::string& enumValuesString){
		//Create the items vector
		std::vector<Type> itemsVec = {};

		//Define the lambda that will handle the data from the enum data parser
		auto vecPopulator = [&itemsVec](const std::string& /*key*/, const int32_t& value){ //key is unused
			//Cast the value as the inner type (which is a member of the outer type), then push it into the vector
			Type casted = static_cast<Type_Val>(value);
			itemsVec.push_back(casted);
		};

		//Call the enum data parser with the raw enum string and the lambda
		parseEnumData(enumValuesString, vecPopulator);

		//Return the filled vector
		return itemsVec;
	}

	inline std::vector<std::string> makeEnumStrVector(const std::string& enumValuesString){
		//Create the names vector
		std::vector<std::string> namesVec = {};

		//Define the lambda that will handle the data from the enum data parser
		auto vecPopulator = [&namesVec](const std::string& key, const int32_t& /*value*/){ //value is unused
			namesVec.push_back(key);
		};

		//Call the enum data parser with the raw enum string and the lambda
		parseEnumData(enumValuesString, vecPopulator);

		//Return the filled vector
		return namesVec;
	}
}

/**
 * @brief The actual template payload that is to be deployed
 * when using this library. Simply initialize with this 
 * definition call, with the 1st argument being the name of
 * the enum class to generate. The remaining arguments are 
 * variadic in nature. Special thanks to marski for providing
 * the vector generation logic for this payload. The relevant
 * StackOverFlow thread can be found here: https://stackoverflow.com/a/57023705
 */
#define einstein_enum(Type, ...) class Type {\
	public:\
		enum Value : int32_t {\
			__VA_ARGS__\
		};\
	\
	private:\
		Value _value;\
		Type(){};\
	public:\
		Type(const Value& value) : _value{value}{}\
	\
	private:\
		static const std::vector<Type> enumItemsVec;\
		static const std::vector<std::string> enumStrVec;\
		static const std::unordered_map<int32_t, std::string> enumNamesMap;\
		static const std::unordered_map<std::string, int32_t> enumValuesMap;\
	public:\
		inline static bool contains(const std::string& value){\
			return enumValuesMap.find(value) != enumValuesMap.end();\
		}\
		inline const std::string name(){\
			return enumNamesMap.at((int32_t) _value);\
		}\
		inline static size_t size(){\
			return enumNamesMap.size();\
		}\
		inline const std::string toString(){\
			return name() + " = " + std::to_string(value());\
		}\
		inline int32_t value(){\
			return ((int32_t) _value);\
		}\
		inline static const std::vector<Type>& values(){\
			return enumItemsVec;\
		}\
		inline static const std::vector<std::string>& valuesStr(){\
			return enumStrVec;\
		}\
	\
	public:\
		inline operator Value() const {\
			return _value;\
		}\
		inline Type& operator=(const Value& newVal){\
			_value = newVal;\
			return *this;\
		}\
		inline bool operator==(const Value& other){\
			return _value == other;\
		}\
		inline bool operator!=(const Value& other){\
			return _value != other;\
		}\
		inline operator std::string(){\
			return name();\
		}\
		inline friend std::ostream& operator<<(std::ostream& outStream, Type& value){\
			outStream << value.name();\
			return outStream;\
		}\
};\
const inline std::vector<Type> Type::enumItemsVec = einstein_enum::makeEnumItemsVec<Type, Type::Value>(#__VA_ARGS__);\
const inline std::vector<std::string> Type::enumStrVec = einstein_enum::makeEnumStrVector(#__VA_ARGS__);\
const inline std::unordered_map<int32_t, std::string> Type::enumNamesMap = einstein_enum::makeEnumNamesMap(#__VA_ARGS__);\
const inline std::unordered_map<std::string, int32_t> Type::enumValuesMap = einstein_enum::makeEnumValuesMap(#__VA_ARGS__);

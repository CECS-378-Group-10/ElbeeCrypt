#pragma once

#include <cstdint>
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
	inline std::string trimWhitespace(std::string str){
		// trim trailing whitespace
		size_t endPos = str.find_last_not_of(" \t");
		if(std::string::npos != endPos){
			str = str.substr(0, endPos + 1);
		}

		// trim leading spaces
		size_t startPos = str.find_first_not_of(" \t");
		if(std::string::npos != startPos){
			str = str.substr(startPos);
		}

		//Allows enum names starting with numbers. Simply prefix the enum with a _ and its string representation will exclude the leading _
		if(str.at(0) == '_' && isdigit(str.at(1))){ //Implements the regex "^_[0-9]{1}" (much faster)
			str = str.substr(1);
		}

		return str;
	}

	inline std::string extractEntry(std::string& valuesString){
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

	inline std::unordered_map<int32_t, std::string> makeEnumNameMap(std::string enumValuesString){
		std::unordered_map<int32_t, std::string> nameMap;

		int32_t currentEnumValue = 0;
		while(enumValuesString != ""){
			std::string currentEnumEntry = extractEntry(enumValuesString);

			size_t equalSignPos = currentEnumEntry.find('=');
			if(equalSignPos != std::string::npos){
				std::string rightHandSide = currentEnumEntry.substr(equalSignPos + 1);
				currentEnumValue = std::stoi(rightHandSide, 0, 0);
				currentEnumEntry.erase(equalSignPos);
			}

			currentEnumEntry = trimWhitespace(currentEnumEntry);

			nameMap[currentEnumValue] = currentEnumEntry;

			currentEnumValue++;
		}

		return nameMap;
	}

	inline std::unordered_map<std::string, int32_t> makeEnumValuesMap(std::string enumValuesString){
		std::unordered_map<std::string, int32_t> nameMap;

		int32_t currentEnumValue = 0;
		while(enumValuesString != ""){
			std::string currentEnumEntry = extractEntry(enumValuesString);

			size_t equalSignPos = currentEnumEntry.find('=');
			if(equalSignPos != std::string::npos){
				std::string rightHandSide = currentEnumEntry.substr(equalSignPos + 1);
				currentEnumValue = std::stoi(rightHandSide);
				currentEnumEntry.erase(equalSignPos);
			}

			currentEnumEntry = trimWhitespace(currentEnumEntry);

			nameMap[currentEnumEntry] = currentEnumValue;

			currentEnumValue++;
		}

		return nameMap;
	}

	inline std::vector<std::string> makeEnumValuesVector(std::string enumStr){
		std::vector<std::string> values = {};

		while(enumStr != ""){
			std::string currentEnumEntry = extractEntry(enumStr);

			size_t equalSignPos = currentEnumEntry.find('=');
			if(equalSignPos != std::string::npos){
				std::string rightHandSide = currentEnumEntry.substr(equalSignPos + 1);
				currentEnumEntry.erase(equalSignPos);
			}

			currentEnumEntry = trimWhitespace(currentEnumEntry);

			values.push_back(currentEnumEntry);
		}

		return values;
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
		static const std::vector<Type> enumItems;\
		static const std::unordered_map<int32_t, std::string> enumNames;\
		static const std::unordered_map<std::string, int32_t> enumValues;\
		static const std::vector<std::string> enumStr;\
	public:\
		inline const std::string& toString(){\
			return enumNames.at((int32_t) _value);\
		}\
		inline static bool contains(const std::string value){\
			return enumValues.find(value) != enumValues.end();\
		}\
		inline static int size(){\
			return enumNames.size();\
		}\
		inline static const std::vector<Type>& values(){\
			return enumItems;\
		}\
		inline static const std::vector<std::string>& valuesStr(){\
			return enumStr;\
		}\
	\
	public:\
		inline operator Value() const {\
			return _value;\
		}\
		inline Type& operator=(Value newVal){\
			_value = newVal;\
			return *this;\
		}\
		inline bool operator==(Value other){\
			return _value == other;\
		}\
		inline bool operator!=(Value other){\
			return _value != other;\
		}\
		inline operator std::string(){\
			return toString();\
		}\
		inline friend std::ostream& operator<<(std::ostream& outStream, Type& value){\
			outStream << value.toString();\
			return outStream;\
		}\
};\
const std::vector<Type> Type::enumItems = {__VA_ARGS__};\
const std::unordered_map<int32_t, std::string> Type::enumNames = einstein_enum::makeEnumNameMap(#__VA_ARGS__);\
const std::unordered_map<std::string, int32_t> Type::enumValues = einstein_enum::makeEnumValuesMap(#__VA_ARGS__);\
const std::vector<std::string> Type::enumStr = einstein_enum::makeEnumValuesVector(#__VA_ARGS__);

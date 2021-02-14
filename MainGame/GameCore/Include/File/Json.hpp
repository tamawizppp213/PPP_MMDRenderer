//////////////////////////////////////////////////////////////////////////////////
///             @file   Json.hpp
///             @brief  Json File Loader and Writer
///             @author Toide Yutaro
///             @date   2021_02_02
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef JSON_HPP
#define JSON_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <map>
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class JsonValue;
using String = std::string;
using Number = double;
using Object = std::map<std::string, JsonValue>;
using Array  = std::vector<JsonValue>;
using Bool   = bool;

//////////////////////////////////////////////////////////////////////////////////
//                         Class, enum
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			JsonValueType
*************************************************************************//**
*  @enum      JsonValueType
*  @brief    Types of json files that can be read
*****************************************************************************/
enum class JsonValueType
{
	String,
	Number,
	Object,
	Array,
	Bool
};

/****************************************************************************
*				  			JsonValue
*************************************************************************//**
*  @class     JsonValue
*  @brief     json file parser and writer.
*****************************************************************************/
class JsonValue
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	JsonValue Parse(const char* data);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	JsonValueType Type;
	union
	{
		String string;
		Number number;
		Object object;
		Array  array;
		Bool   boolean;
		
	};
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	JsonValue();
	~JsonValue();
	JsonValue(const JsonValue& jsonValue);
	JsonValue(const std::string& str);
	JsonValue(double value);
	JsonValue(const Object& obj);
	JsonValue(const Array& arr);
	JsonValue(bool b);

	template<typename T>
	JsonValue& operator=(const T& value)
	{
		(*this).~JsonValue;
		new(this) JsonValue(value);
		return *this;
	}


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	JsonValue ParseJsonValue(const char*& data);
	JsonValue ParseString(const char*& data);
	JsonValue ParseNumber(const char*& data);
	JsonValue ParseObject(const char*& data);
	JsonValue ParseArray (const char*& data);
	JsonValue ParseBool  (const char*& data);
	void SkipSpace(const char*& data);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif
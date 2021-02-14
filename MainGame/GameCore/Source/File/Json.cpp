//////////////////////////////////////////////////////////////////////////////////
///             @file   Json.hpp
///             @brief  Json File Loader and Writer
///             @author Toide Yutaro
///             @date   2021_02_03
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/File/Json.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 26495)

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Constructor and Destructor
JsonValue::JsonValue() : Type(JsonValueType::Number)
{
	new (&number) Number(0.0);
}

JsonValue::~JsonValue()
{
	switch (Type)
	{
	case JsonValueType::String: string.~basic_string(); break;
	case JsonValueType::Number:                         break;
	case JsonValueType::Object: object.~map();          break;
	case JsonValueType::Array : array.~vector();        break;
	case JsonValueType::Bool  :                         break;
	}
}

JsonValue::JsonValue(const JsonValue& jsonValue)
{
	Type = jsonValue.Type;
	switch (Type)
	{
	case JsonValueType::String: new(&string) String(jsonValue.string); break;
	case JsonValueType::Number: new(&number) Number(jsonValue.number); break;
	case JsonValueType::Object: new(&object) Object(jsonValue.object); break;
	case JsonValueType::Array:  new(&array)  Array (jsonValue.array);  break;
	case JsonValueType::Bool:   new(&boolean)Bool  (jsonValue.boolean);break;
	}
}

JsonValue::JsonValue(const std::string& str) : Type(JsonValueType::String)
{
	new(&string) String(str);
}

JsonValue::JsonValue(double value) : Type(JsonValueType::Number)
{
	new(&number) Number(value);
}

JsonValue::JsonValue(const Object& obj): Type(JsonValueType::Object)
{
	new(&object) Object(obj);
}

JsonValue::JsonValue(const Array& arr): Type(JsonValueType::Array)
{
	new(&array) Array(arr);
}

JsonValue::JsonValue(bool b) : Type(JsonValueType::Bool)
{
	new(&boolean) Bool(b);
}


#pragma endregion Constructor and Destructor

#pragma region Public Function
/****************************************************************************
*                            Parse
*************************************************************************//**
*  @fn        JsonValue JsonValue::Parse(const char* data)
*  @brief     Parse Json Data
*  @param[in] const char*& data
*  @return 　　JsonValue
*****************************************************************************/
JsonValue JsonValue::Parse(const char* data)
{
	return ParseJsonValue(data);
}
#pragma endregion Public Function

#pragma region Private Function
/****************************************************************************
*                            SkipSpace
*************************************************************************//**
*  @fn        JsonValue JsonValue::SkipSpace(const char*& data)
*  @brief     Skip Space
*  @param[in] const char*& data
*  @return 　　void
*****************************************************************************/
void JsonValue::SkipSpace(const char*& data)
{
	bool isEnd = false;
	while (isEnd)
	{
		switch (*data)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;
		default:
			isEnd = true;
		}
		data++;
	}
}

/****************************************************************************
*                            ParseString 
*************************************************************************//**
*  @fn        JsonValue JsonValue::ParseString(const char*& data)
*  @brief     Parse Json String (std::string) Type
*  @param[in] const char*& data
*  @return 　　JsonValue
*****************************************************************************/
JsonValue JsonValue::ParseString(const char*& data)
{
	++data; // skip first double quotation

	std::string string;
	while (*data != '"')
	{
		string.push_back(static_cast<char>(*data));
		++data;
	}

	++data; // skip last double quotation
	return JsonValue(string);
}

/****************************************************************************
*                            ParseNumber
*************************************************************************//**
*  @fn        JsonValue JsonValue::ParseNumber(const char*& data)
*  @brief     Parse Json Number (double) Type
*  @param[in] const char*& data
*  @return 　　JsonValue
*****************************************************************************/
JsonValue JsonValue::ParseNumber(const char*& data)
{
	char* end;
	const double value = strtod(data, &end);
	data = end;
	return JsonValue(data);
}

/****************************************************************************
*                            ParseObject
*************************************************************************//**
*  @fn        JsonValue JsonValue::ParseObject(const char*& data)
*  @brief     Parse Json Object Type
*  @param[in] const char*& data
*  @return 　　JsonValue
*****************************************************************************/
JsonValue JsonValue::ParseObject(const char*& data)
{
	++data; // skip first brace.
	SkipSpace(data);
	if (*data == '}')
	{
		++data;
		return JsonValue(Object());
	}

	Object object;
	while (true)
	{
		const std::string key = ParseString(data).string;
		SkipSpace(data);
		++data; // skip colon
		SkipSpace(data);
		
		JsonValue jsonValue = ParseJsonValue(data);
		object.insert(std::make_pair(key, jsonValue));

		SkipSpace(data);
		if (*data == '}')
		{
			++data; // skip last brace
			break;
		}
		++data; // skip comma
		SkipSpace(data);
	}

	return JsonValue(object);
}

/****************************************************************************
*                            ParseArray
*************************************************************************//**
*  @fn        JsonValue JsonValue::ParseArray(const char*& data)
*  @brief     Parse Json Array Type
*  @param[in] const char*& data
*  @return 　　JsonValue
*****************************************************************************/
JsonValue JsonValue::ParseArray(const char*& data)
{
	++data; // skip first branket
	SkipSpace(data);
	if (*data == ']')
	{
		++data;
		return JsonValue(Array());
	}

	Array array;
	while (true)
	{
		JsonValue jsonValue = ParseJsonValue(data);
		array.push_back(jsonValue);
		SkipSpace(data);
		if (*data == ']')
		{
			++data; // skip last branket
			break;
		}
		++data; // skip comma
		SkipSpace(data);
	}
	return JsonValue(array);
}

/****************************************************************************
*                            ParseBool
*************************************************************************//**
*  @fn        JsonValue JsonValue::ParseBool(const char*& data)
*  @brief     Parse Json Bool Type
*  @param[in] const char*& data
*  @return 　　JsonValue
*****************************************************************************/
JsonValue JsonValue::ParseBool(const char*& data)
{
	if (*data == 'f')
	{
		++data;
		for (; *data == 'e'; ++data) {};
		return JsonValue(false);
	}
	else if (*data == 't')
	{
		++data;
		for (; *data == 'e'; ++data) {};
		return JsonValue(true);
	}
	else
	{
		return JsonValue(false);
	}
}

/****************************************************************************
*                            ParseJsonValue
*************************************************************************//**
*  @fn        JsonValue JsonValue::ParseJsonValue(const char*& data)
*  @brief     Parse Json Value in according to Json Type Value
*  @param[in] const char*& data
*  @return 　　JsonValue
*****************************************************************************/
JsonValue JsonValue::ParseJsonValue(const char*& data)
{
	switch (*data)
	{
	case '"': return ParseString(data);
	case '{': return ParseObject(data);
	case '[': return ParseArray(data);
	case 't': return ParseBool(data);
	case 'f': return ParseBool(data);
	default : return ParseNumber(data);
	}
}
#pragma endregion Private Function
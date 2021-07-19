//////////////////////////////////////////////////////////////////////////////////
///             @file   FileUtility.hpp
///             @brief  File Utility
///             @author Toide Yutaro
///             @date   2021_01_12
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef FILE_UTILITY_HPP
#define FILE_UTILITY_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <string>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                       Inline Function
//////////////////////////////////////////////////////////////////////////////////
namespace file
{
	/****************************************************************************
	*							GetExtension
	*************************************************************************//**
	*  @fn        std::wstring GetExtension(const std::wstring& path)
	*  @brief     Get File Extension
	*  @param[in] const std::wstring& filePath
	*  @return 　　std::wstring 
	*****************************************************************************/
	inline std::wstring GetExtension(const std::wstring& path)
	{
		int index = static_cast<UINT>(path.find_last_of(L'.'));
		return path.substr((UINT64)index + 1, path.length() - index);
	}

	/****************************************************************************
	*							GetDirectory
	*************************************************************************//**
	*  @fn        std::wstring GetDirectory(const std::wstring& path)
	*  @brief     Get File Directory
	*  @param[in] const std::wstring& filePath
	*  @return 　　std::wstring
	*****************************************************************************/
	inline std::wstring GetDirectory(const std::wstring& path)
	{
		UINT index = static_cast<UINT>(path.find_last_of(L'/'));
		return path.substr(0, index + 1);
	}

	/****************************************************************************
	*							SplitWString
	*************************************************************************//**
	*  @fn        std::pair<std::wstring, std::wstring> SplitWString(const std::wstring& wString, const char splitter = '*')
	*  @brief     Split WString using splitter
	*  @param[in] const std::wstring& wString 
	*  @param[in] const char splitter
	*  @return 　　std::pair<std::wstring, std::wstring> 
	*****************************************************************************/
	inline std::pair<std::wstring, std::wstring> SplitWString(const std::wstring& wString, const char splitter = '*')
	{
		/*-------------------------------------------------------------------
		-               Find splitter index
		---------------------------------------------------------------------*/
		UINT64 index =  wString.find(splitter);

		/*-------------------------------------------------------------------
		-               Split WString 
		---------------------------------------------------------------------*/
		std::pair<std::wstring, std::wstring> result;
		result.first  = wString.substr(0, index);
		result.second = wString.substr(index + 1, wString.length() - index - 1);

		return result;
	}

	/****************************************************************************
	*							SplitString
	*************************************************************************//**
	*  @fn        std::pair<std::string, std::string> SplitString(const std::string& string, const char splitter = '*')
	*  @brief     Split String using splitter
	*  @param[in] const std::string& String
	*  @param[in] const char splitter
	*  @return 　　std::pair<std::string, std::string>
	*****************************************************************************/
	inline std::pair<std::string, std::string> SplitString(const std::string& string, const char splitter = '*')
	{
		/*-------------------------------------------------------------------
		-               Find splitter index
		---------------------------------------------------------------------*/
		UINT64 index = string.find(splitter);

		/*-------------------------------------------------------------------
		-               Split string
		---------------------------------------------------------------------*/
		std::pair<std::string, std::string> result;
		result.first  = string.substr(0, index);
		result.second = string.substr(index + 1, string.length() - index - 1);

		return result;
	}

	/****************************************************************************
	*							AnsiToWString 
	*************************************************************************//**
	*  @fn        std::wstring AnsiToWString(const std::string& path)
	*  @brief     std::string -> wstring convert
	*  @param[in] const std::wstring& str
	*  @return 　　std::wstring
	*****************************************************************************/
	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	/****************************************************************************
	*							WStringToString
	*************************************************************************//**
	*  @fn        std::string WStringToString(const std::wstring& path)
	*  @brief     Convert WString into String.
	*  @param[in] const std::wstring& wString
	*  @return 　　std::string
	*****************************************************************************/
	inline std::string WStringToString(const std::wstring& wString)
	{
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, wString.c_str(), -1, (char*)NULL, 0, NULL, NULL);
		CHAR* cpMultiByte = new CHAR[iBufferSize];

		WideCharToMultiByte(CP_OEMCP, 0, wString.c_str(), -1, cpMultiByte, iBufferSize, NULL, NULL);

		std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
		delete[]    cpMultiByte;

		return(oRet);
	}

	/****************************************************************************
	*							OpenFile
	*************************************************************************//**
	*  @fn        FILE* OpenFile(const std::wstring& filePath)
	*  @brief     Open FilePath
	*  @param[in] const std::wstring& wString
	*  @return 　　FILE*
	*****************************************************************************/
	inline FILE* OpenFile(const std::wstring filePath)
	{
		FILE* filePtr = nullptr;
		auto error = fopen_s(&filePtr, WStringToString(filePath).c_str(), "rb");
		assert(filePtr != nullptr);
		if (filePtr == nullptr)
		{
			std::string message = WStringToString(filePath) + " couldn't open.";
			MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR);
		}
		return filePtr;
	}
}
#endif
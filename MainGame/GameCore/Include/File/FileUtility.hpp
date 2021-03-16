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
	inline std::wstring GetExtension(const std::wstring& path)
	{
		int index = static_cast<UINT>(path.find_last_of(L'.'));
		return path.substr((UINT64)index + 1, path.length() - index);
	}

	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	inline std::string WStringToString(const std::wstring& wString)
	{
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, wString.c_str(), -1, (char*)NULL, 0, NULL, NULL);
		CHAR* cpMultiByte = new CHAR[iBufferSize];

		WideCharToMultiByte(CP_OEMCP, 0, wString.c_str(), -1, cpMultiByte, iBufferSize, NULL, NULL);

		std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
		delete[]    cpMultiByte;

		return(oRet);
	}

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
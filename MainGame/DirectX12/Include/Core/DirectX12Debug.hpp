//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12Debug.hpp
//            Content:  DirectX12 Debugger Helper
//             Author:  Toide Yutaro
//             Create:  2020_11_10
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_DEBUG_HPP
#define DIRECTX12_DEBUG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <stdexcept>
#include <Windows.h>
#include <comdef.h>
#include <string>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         HResult Exception Class
//////////////////////////////////////////////////////////////////////////////////
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

inline std::string WStringToString(std::wstring wString)
{
	int iBufferSize   = WideCharToMultiByte(CP_OEMCP, 0, wString.c_str(), -1, (char*)NULL, 0, NULL, NULL);
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
class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hresult, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
	{
		ErrorCode    = hresult;
		FunctionName = functionName;
		Filename     = filename;
		LineNumber   = lineNumber;

		// Get the string description of the error code.
		MessageBoxA(NULL, WStringToString(ToString()).c_str(), "Error", MB_ICONERROR);
	}

	inline std::wstring ToString()
	{
		// Get the string description of the error code.
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();
		std::wstring allErrorMessage = FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
		
		return allErrorMessage;
	}

	HRESULT      ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int          LineNumber = -1;
};

#define SAFE_RELEASE(p) {if(p){(p)->Release(); p = nullptr;}}

//////////////////////////////////////////////////////////////////////////////////
//                         ThrowIfFailed
//                Always use this function when hresult is returned 
//////////////////////////////////////////////////////////////////////////////////
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#endif

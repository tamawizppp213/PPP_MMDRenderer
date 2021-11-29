//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12Sharder.hpp
///             @brief  DirectX12 Compile Shader
///             @author Toide Yutaro
///             @date   2020_11_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Core.hpp"
#include <fstream>
#include <d3dcompiler.h>
#include <dxcapi.h>
#include <filesystem>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct SimpleGraphicPipeline
{
	BlobComPtr VertexShader;
	BlobComPtr PixelShader;
};

//////////////////////////////////////////////////////////////////////////////////
//                         DirectX12 Shader Function (inline)
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*							CompileShader
*************************************************************************//**
*  @fn        BlobComPtr CompileShader(const std::wstring& fileName,
                   const D3D_SHADER_MACRO* defines,const std::string& 
				   entrypoint,const std::string& target)
*  @brief     Compile Shader (Shader Model Under 5.x)
*  @param[in] std::wstring& fileName : filePath
*  @param[in] D3D_SHADER_MACRO& defines
*  @param[in] std::string& entrypoint,
*  @param[in] std::string& target (ex. ps5_0, vs5_1...)
*  @return 　　BlobComPtr
*****************************************************************************/
inline BlobComPtr CompileShader(
	const std::wstring& fileName,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hresult = S_OK;

	BlobComPtr byteCode = nullptr;
	BlobComPtr errors;
	hresult = D3DCompileFromFile(fileName.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}

	ThrowIfFailed(hresult);

	return byteCode;
}

/****************************************************************************
*							CompileShader
*************************************************************************//**
*  @fn        BlobComPtr CompileShader(const std::wstring& fileName,
				   const std::wstring& entrypoint, const std::wstring& target)
*  @brief     Compile Shader (Shader Model Over 6.0)
*  @param[in] std::wstring& fileName : filePath
*  @param[in] std::wstring& entrypoint,
*  @param[in] std::wstring& target (ex. ps6_0, vs6_1...)
*  @return 　　BlobComPtr
*****************************************************************************/
inline BlobComPtr CompileShader(
	const std::wstring& fileName,
	const std::wstring& entrypoint,
	const std::wstring& target)
{
	std::filesystem::path filePath = fileName;
	std::ifstream infile(filePath);
	std::vector<char> sourceData;

	if (!infile) { throw std::runtime_error("shader not found"); }
	sourceData.resize(uint32_t(infile.seekg(0, infile.end).tellg()));
	infile.seekg(0, infile.beg).read(sourceData.data(), sourceData.size());

	// Compile using DXC
	ComPtr<IDxcLibrary>         library;
	ComPtr<IDxcCompiler>        compiler;
	ComPtr<IDxcBlobEncoding>    source;
	ComPtr<IDxcOperationResult> dxcResult;

	DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	library->CreateBlobWithEncodingFromPinned(sourceData.data(), UINT(sourceData.size()), CP_ACP, &source);
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

	LPCWSTR compilerFlags[] = {
#if _DEBUG
	L"/Zi", L"/O0",
#else
	L"/O2"
#endif
	};

	HRESULT hresult     = S_OK;
	BlobComPtr byteCode = nullptr;
	BlobComPtr errors   = nullptr;

	hresult = compiler->Compile(
		source.Get(), 
		filePath.wstring().c_str(),
		entrypoint.c_str(),
		target.c_str(),
		compilerFlags, _countof(compilerFlags),
		nullptr, 0, // Defines
		nullptr,
		&dxcResult);

	if (SUCCEEDED(hresult))
	{
		dxcResult->GetResult(reinterpret_cast<IDxcBlob**>(byteCode.GetAddressOf()));
	}
	else
	{
		dxcResult->GetErrorBuffer(reinterpret_cast<IDxcBlobEncoding**>(errors.GetAddressOf()));
	}
	ThrowIfFailed(hresult);

	return byteCode;

}

/****************************************************************************
*							LoadBinary
*************************************************************************//**
*  @fn        BlobComPtr LoadBinary(const std::wstring& fileName)
*  @brief     Load Binary Data (Done Compile)
*  @param[in] std::wstring& fileName : filePath
*  @return 　　BlobComPtr
*****************************************************************************/
inline BlobComPtr LoadBinary(const std::wstring& filename)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	BlobComPtr blob;
	ThrowIfFailed(D3DCreateBlob(size, blob.GetAddressOf()));

	fin.read((char*)blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}

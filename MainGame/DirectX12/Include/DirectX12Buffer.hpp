//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12Buffer.hpp
//            Content:  DirectX12 Buffer Class  
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_BUFFER_HPP
#define DIRECTX12_BUFFER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Core.hpp"
#include "DirectX12Debug.hpp"
#include "DirectX12BaseStruct.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
static inline UINT CalcConstantBufferByteSize(UINT byteSize)
{
	return (byteSize + 255) & ~255;
}

/****************************************************************************
*							 DefaultBuffer
*************************************************************************//**
*  @class     Default Buffer
*  @brief     Default Buffer (Constant buffer elements need to be multiples of 256 bytes)
*  @details   Constant buffer elements need to be multiples of 256 bytes.
              This is because the hardware can only view constant data 
              at m*256 byte offsets and of n*256 byte lengths. 
              typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
              UINT64 OffsetInBytes; // multiple of 256
              UINT   SizeInBytes;   // multiple of 256
              D3D12_CONSTANT_BUFFER_VIEW_DESC; 
*****************************************************************************/
struct DefaultBuffer
{
public:
	explicit DefaultBuffer(Device* device, CommandList* commandList, const void* initData, UINT64 totalByteSize, ResourceComPtr& uploadBuffer);

	inline ResourceComPtr Resource() const
	{
		return _defaultBuffer.Get();
	}

	DefaultBuffer(const DefaultBuffer& defaultBuffer)            = delete;
	DefaultBuffer& operator=(const DefaultBuffer& defaultBuffer) = delete;
	~DefaultBuffer();

private:
	ResourceComPtr _defaultBuffer;
	UINT64         _totalByteSize = 0;
};

/****************************************************************************
*							 UploadBuffer
*************************************************************************//**
*  @struct    Upload Buffer
*  @brief     Upload Buffer
*****************************************************************************/
template<typename T>
struct UploadBuffer
{
public:
	UploadBuffer(Device* device, UINT elementCount, bool isConstantBuffer) : _isConstantBuffer(isConstantBuffer)
	{
		_elementByteSize = sizeof(T);

		// Constant buffer elements need to be multiples of 256 bytes.
		if (_isConstantBuffer)
		{
			_elementByteSize = CalcConstantBufferByteSize(sizeof(T));
		}

		ThrowIfFailed(device->CreateCommittedResource(
			&HEAP_PROPERTY(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&RESOURCE_DESC::Buffer(_elementByteSize * elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_uploadBuffer)));

		ThrowIfFailed(_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(_mappedData)));
	}

	UploadBuffer(const UploadBuffer& rhs)            = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer()
	{
		if (_uploadBuffer != nullptr)
		{
			_uploadBuffer->Unmap(0, nullptr);
		}

		_mappedData = nullptr;
	}

	inline Resource* Resource() const
	{
		return _uploadBuffer.Get();
	}

	inline void CopyData(int elementIndex, const T& data)
	{
		memcpy(&_mappedData[elementIndex * _elementByteSize], &data, sizeof(T));
	}

private:
	ResourceComPtr _uploadBuffer;
	BYTE* _mappedData       = nullptr;
	UINT  _elementByteSize  = 0;
	bool  _isConstantBuffer = false;
};
#endif

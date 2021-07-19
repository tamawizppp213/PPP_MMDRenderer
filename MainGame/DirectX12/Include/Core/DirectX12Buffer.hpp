//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12Buffer.hpp
//            Content:  DirectX12 Buffer Class  
//             Author:  Toide Yutaro
//             Create:  2020_11_16
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_BUFFER_HPP
#define DIRECTX12_BUFFER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12BaseStruct.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
static inline UINT CalcConstantBufferByteSize(UINT byteSize)
{
	return (byteSize + 255) & ~255;
}

static inline UINT AlignmentValue(UINT size, UINT alignment)
{
	return (size + alignment - (size % alignment));
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

	DefaultBuffer(const DefaultBuffer& defaultBuffer)            = default;
	DefaultBuffer& operator=(const DefaultBuffer& defaultBuffer) = default;
	~DefaultBuffer();

private:
	ResourceComPtr _defaultBuffer = nullptr;
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

		auto heapProp = HEAP_PROPERTY(D3D12_HEAP_TYPE_UPLOAD);
		auto buffer   = RESOURCE_DESC::Buffer((UINT64)_elementByteSize * elementCount);
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_uploadBuffer)));

	}

	UploadBuffer(const UploadBuffer& rhs)            = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer(){}

	inline ID3D12Resource1* Resource() const
	{
		return _uploadBuffer.Get();
	}

	inline void CopyStart()
	{
		ThrowIfFailed(_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedData)));
	}

	inline void CopyData(int elementIndex, const T& data)
	{
		std::memcpy(&_mappedData[elementIndex * _elementByteSize], &data, sizeof(T));
	}

	inline void CopyTotalData(const T* data, int dataLength)
	{
		std::memcpy(&_mappedData[0], data, sizeof(T) * dataLength);
	}

	inline void CopyEnd()
	{
		_uploadBuffer->Unmap(0, nullptr);
	}

private:
	ResourceComPtr _uploadBuffer;
	BYTE* _mappedData       = nullptr;
	UINT  _elementByteSize  = 0;
	bool  _isConstantBuffer = false;
};

struct StructuredBuffer
{
public:
	~StructuredBuffer();

	void Initialize(int elementByteSize, int elementCount, void* initData);
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	void Update(void* data);
	bool IsInited() const { return _isInitialized; }
	Resource* GetResource();
private:
	Resource* _bufferOnGPU = nullptr;
	void*     _bufferOnCPU = nullptr;
	int _elementCount      = 0;
	int _elementByteSize   = 0;
	bool _isInitialized    = false;
};

struct RWStructuredBuffer
{
public:
	~RWStructuredBuffer();
	
	void Initialize(int elementByteSize, int elementCount, void* initData);
	void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	bool IsInitialized() const { return _isInitialized; }
	void* GetResourceOnCPU();
	Resource* GetResource();

private:
	Resource* _bufferOnGPU = nullptr;
	void* _bufferOnCPU     = nullptr;
	int _elementCount      = 0;
	int _elementByteSize   = 0;
	bool _isInitialized     = false;
};
#endif

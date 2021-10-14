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
#include "DirectX12Texture.hpp"
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


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			Color Buffer
*************************************************************************//**
*  @class     Color Buffer
*  @brief     Create color buffer class
*****************************************************************************/
class ColorBuffer
{
	enum class ResourceID
	{
		RTV,
		SRV,
		UAV,
		CountOfResourceType
	};
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Create(int width, int height, DXGI_FORMAT colorFormat =DXGI_FORMAT_R8G8B8A8_UNORM, float clearColor[4]= nullptr);
	bool OnResize(int newWidth, int newHeight);
	bool ClearBuffer();
	bool CopyToColorBuffer(Resource* source, D3D12_RESOURCE_STATES sourceState = D3D12_RESOURCE_STATE_COMMON);
	bool TransitionResourceState(D3D12_RESOURCE_STATES after);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSRV() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPURTV() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUUAV() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRV() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPURTV() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUUAV() const;
	Texture& GetColorBuffer() { return _colorBuffer; }
	D3D12_RESOURCE_STATES GetUsageState() const { return _usageState; }
	DXGI_FORMAT GetFormat()      const { return _format; }
	const float* GetClearColor() const { return _clearColor; }
	gm::Float2 GetTextureSize() const { return _colorBuffer.ImageSize; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	ColorBuffer() = default;
	~ColorBuffer();
	ColorBuffer(const ColorBuffer&)            = delete;
	ColorBuffer& operator=(const ColorBuffer&) = delete;
	ColorBuffer(ColorBuffer&&)                 = default;
	ColorBuffer& operator=(ColorBuffer&&)      = default;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool CreateDescriptorHeap();
	bool CreateTexture(int width, int height, DXGI_FORMAT format, float clearColor[4]);
	bool CreateDescriptor(DXGI_FORMAT format);
	bool IssueViewID();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Texture _colorBuffer;
	UINT    _resourceID[(int)ResourceID::CountOfResourceType]; 
	DXGI_FORMAT _format;
	float   _clearColor[4];
	bool    _isInitialzed = false;
	D3D12_RESOURCE_STATES _usageState;
};

/****************************************************************************
*				  			RWStructuredBuffer
*************************************************************************//**
*  @class     RWStructured Buffer
*  @brief     RW Structured Buffer
*****************************************************************************/
class RWStructuredBuffer
{
	enum class ResourceID
	{
		SRV,
		UAV,
		CountOfResourceType
	};

public:
	/**************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Create(int elementByteSize, int elementCount);
	bool OnResize(int elementCount);
	inline void CopyStart()                                 { ThrowIfFailed(_buffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedData)));}
	inline void CopyData(int elementIndex, const void* data){ std::memcpy(&_mappedData[elementIndex * _elementByteSize], data, _elementByteSize);}
	inline void CopyTotalData(const void* data)             { std::memcpy(&_mappedData[0], data, _elementByteSize * _elementCount); }
	inline void CopyEnd(){ _buffer->Unmap(0, nullptr); }

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSRV() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUUAV() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRV() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUUAV() const;
	Resource* GetResource() { return _buffer.Get(); }
	bool IsCreated() const { return _isInitialized; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	RWStructuredBuffer() = default;
	~RWStructuredBuffer() {};
	RWStructuredBuffer(const RWStructuredBuffer&)            = delete; // prohibit copy
	RWStructuredBuffer& operator=(const RWStructuredBuffer&) = delete; // prohibit copy
	RWStructuredBuffer(RWStructuredBuffer&&)                 = default;
	RWStructuredBuffer& operator=(RWStructuredBuffer&&)      = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool IssueViewID();
	bool PrepareDescriptor();
	bool PrepareResource();
	

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	ResourceComPtr _buffer = nullptr;
	BYTE* _mappedData      = nullptr;
	UINT  _elementByteSize = 0;
	UINT  _elementCount    = 0;
	bool _isInitialized    = false;
	UINT _resourceID[(int)ResourceID::CountOfResourceType];
};


#endif

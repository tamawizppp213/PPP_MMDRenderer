//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12Buffer.cpp
//            Content:  DirectX12 Buffer Class  
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*							Constructor
*************************************************************************//**
*****************************************************************************/
DefaultBuffer::DefaultBuffer(Device* device, CommandList* commandList, const void* initData, UINT64 totalByteSize, ResourceComPtr& uploadBuffer)
{
	_totalByteSize = totalByteSize;

	/*-------------------------------------------------------------------
	-           Create the actual default buffer resource
	---------------------------------------------------------------------*/
	auto defaultHeapProp = HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceBuffer  = RESOURCE_DESC::Buffer(totalByteSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceBuffer,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(_defaultBuffer.GetAddressOf())));

	/*-------------------------------------------------------------------
	-          Copy CPU memory data into our dafault buffer, 
	-          we need to create an intermediate upload heap
	---------------------------------------------------------------------*/
	auto uploadHeapProp   = HEAP_PROPERTY(D3D12_HEAP_TYPE_UPLOAD);
	auto uploadTempBuffer = RESOURCE_DESC::Buffer(totalByteSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&uploadTempBuffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	/*-------------------------------------------------------------------
	-      Describe the data we want to copy into the default buffer.
	---------------------------------------------------------------------*/
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData      = initData;
	subResourceData.RowPitch   = totalByteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	/*-------------------------------------------------------------------
	-      Schedule to copy the data to the default buffer resource
	---------------------------------------------------------------------*/
	auto before = BARRIER::Transition(_defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,    D3D12_RESOURCE_STATE_COPY_DEST);
	commandList->ResourceBarrier(1, &before);
	UpdateSubresources<1>(commandList, _defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	auto after = BARRIER::Transition(_defaultBuffer.Get(),  D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	commandList->ResourceBarrier(1, &after);
}

DefaultBuffer::~DefaultBuffer()
{
	
}

StructuredBuffer::~StructuredBuffer()
{
	if (_bufferOnGPU)
	{
		_bufferOnGPU->Unmap(0, nullptr);
		_bufferOnGPU->Release();
	}
}

void StructuredBuffer::Initialize(int elementByteSize, int elementCount, void* initData)
{
	_elementByteSize = elementByteSize;
	_elementCount    = elementCount;
	
	Device* device       = DirectX12::Instance().GetDevice();
	auto defaultHeapProp = HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceBuffer  = RESOURCE_DESC::Buffer(_elementByteSize * elementCount);
	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceBuffer,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_bufferOnGPU)));

	_bufferOnGPU->Map(0, nullptr, reinterpret_cast<void**>(&_bufferOnCPU));
	if (initData != nullptr)
	{
		memcpy(_bufferOnCPU, initData, _elementByteSize * _elementCount);
	}
	_isInitialized = true;
}

void StructuredBuffer::Update(void* data)
{
	memcpy(_bufferOnCPU, data, _elementByteSize * _elementCount);
}

Resource* StructuredBuffer::GetResource()
{
	return _bufferOnGPU;
}

void StructuredBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	if (!_isInitialized) { return; }
	Device* device = DirectX12::Instance().GetDevice();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format                     = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement        = 0;
	srvDesc.Buffer.NumElements         = static_cast<UINT>(_elementCount);
	srvDesc.Buffer.StructureByteStride = _elementByteSize;
	srvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(
		_bufferOnGPU, &srvDesc, descriptorHandle);
}

#pragma region RWStructuredBuffer
RWStructuredBuffer::~RWStructuredBuffer()
{
	if (_bufferOnGPU)
	{
		_bufferOnGPU->Unmap(0, nullptr);
		_bufferOnGPU->Release();
	}
}

void RWStructuredBuffer::Initialize(int elementByteSize, int elementCount, void* initData)
{
	_elementByteSize = elementByteSize;
	_elementCount    = elementCount;
	
	Device* device       = DirectX12::Instance().GetDevice();
	auto resourceBuffer  = RESOURCE_DESC::Buffer(_elementByteSize * elementCount);
	resourceBuffer.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES defaultHeapProp;
	defaultHeapProp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	defaultHeapProp.CreationNodeMask     = 1;
	defaultHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	defaultHeapProp.VisibleNodeMask      = 1;

	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceBuffer,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&_bufferOnGPU)));

	_bufferOnGPU->Map(0, nullptr, reinterpret_cast<void**>(&_bufferOnCPU));
	if (initData != nullptr)
	{
		memcpy(_bufferOnCPU, initData, _elementByteSize * _elementCount);
	}
	_isInitialized = true;
}

void* RWStructuredBuffer::GetResourceOnCPU()
{
	return _bufferOnCPU;
}

Resource* RWStructuredBuffer::GetResource()
{
	return _bufferOnGPU;
}


void RWStructuredBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	if (!_isInitialized) { return; }
	Device* device = DirectX12::Instance().GetDevice();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(_elementCount);
	srvDesc.Buffer.StructureByteStride = _elementByteSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(
		_bufferOnGPU, &srvDesc, descriptorHandle);
}


void RWStructuredBuffer::RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	if (!_isInitialized) { return; }
	Device* device = DirectX12::Instance().GetDevice();

	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements = static_cast<UINT>(_elementCount);
	desc.Buffer.StructureByteStride = _elementByteSize;
	device->CreateUnorderedAccessView(
		_bufferOnGPU,
		nullptr,
		&desc,
		descriptorHandle
	);
}

#pragma endregion RWStructuredBuffer
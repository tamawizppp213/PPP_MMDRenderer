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

#pragma region Color Buffer
ColorBuffer::~ColorBuffer()
{

}

#pragma region Public Function
bool ColorBuffer::Create(int width, int height, DXGI_FORMAT colorFormat, float clearColor[4])
{
	if (_isInitialzed) { return true; }

	if (!IssueViewID()) { MessageBox(NULL, L"Failed to issue view ID.", L"Warning", MB_ICONWARNING); return false; }
	if (!CreateTexture(width, height, colorFormat, clearColor))
	{
		MessageBox(NULL, L"Failed to create texture resource.", L"Warning", MB_ICONWARNING);
		return false;
	}
	if (!CreateDescriptor(colorFormat))
	{
		MessageBox(NULL, L"Failed to create descriptor.", L"Warning", MB_ICONWARNING);
		return false;
	}
	_format = colorFormat;
	_isInitialzed = true;
	return true;
}

bool ColorBuffer::ClearBuffer()
{
	
	return true;
}

bool ColorBuffer::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer.ImageSize.x != newWidth ||
		_colorBuffer.ImageSize.y != newHeight)
	{
		CreateTexture(newWidth, newHeight, _colorBuffer.Format, _clearColor);
		CreateDescriptor(_colorBuffer.Format);
	}
	return true;
}

bool ColorBuffer::CopyToColorBuffer(Resource* source, D3D12_RESOURCE_STATES sourceState)
{
	CommandList* commandList = DirectX12::Instance().GetCommandList();

	BARRIER barrier1[] =
	{
		BARRIER::Transition(source,
		sourceState, D3D12_RESOURCE_STATE_COPY_SOURCE),
		BARRIER::Transition(_colorBuffer.Resource.Get(),
		_usageState, D3D12_RESOURCE_STATE_COPY_DEST)
	};

	commandList->ResourceBarrier(_countof(barrier1), barrier1);
	commandList->CopyResource(_colorBuffer.Resource.Get(), source);
	
	BARRIER barrier2[] =
	{
		BARRIER::Transition(source,
		D3D12_RESOURCE_STATE_COPY_SOURCE, sourceState),
		BARRIER::Transition(_colorBuffer.Resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, _usageState)
	};
	commandList->ResourceBarrier(_countof(barrier2), barrier2);
	return true;
}

bool ColorBuffer::TransitionResourceState(D3D12_RESOURCE_STATES after)
{
	if (after == _usageState) { return false; }

	CommandList* commandList = DirectX12::Instance().GetCommandList();
	BARRIER barrier[] =
	{
		BARRIER::Transition(_colorBuffer.Resource.Get(),
		_usageState, after)
	};
	commandList->ResourceBarrier(1, barrier);
	_usageState = after;
	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE ColorBuffer::GetCPUSRV() const
{
	return DirectX12::Instance().GetCPUResourceView(HeapType::SRV, _resourceID[(int)ResourceID::SRV]);
}

D3D12_CPU_DESCRIPTOR_HANDLE ColorBuffer::GetCPURTV() const
{
	return DirectX12::Instance().GetCPUResourceView(HeapType::RTV, _resourceID[(int)ResourceID::RTV]);
}
D3D12_CPU_DESCRIPTOR_HANDLE ColorBuffer::GetCPUUAV() const
{
	return DirectX12::Instance().GetCPUResourceView(HeapType::UAV, _resourceID[(int)ResourceID::UAV]);
}
D3D12_GPU_DESCRIPTOR_HANDLE ColorBuffer::GetGPUSRV() const
{
	return DirectX12::Instance().GetGPUResourceView(HeapType::SRV, _resourceID[(int)ResourceID::SRV]);
}
D3D12_GPU_DESCRIPTOR_HANDLE ColorBuffer::GetGPURTV() const
{
	return DirectX12::Instance().GetGPUResourceView(HeapType::RTV, _resourceID[(int)ResourceID::RTV]);
}
D3D12_GPU_DESCRIPTOR_HANDLE ColorBuffer::GetGPUUAV() const
{
	return DirectX12::Instance().GetGPUResourceView(HeapType::UAV, _resourceID[(int)ResourceID::UAV]);
}
#pragma endregion Public Function
#pragma region Private Function
bool ColorBuffer::CreateDescriptorHeap()
{
	// future work
	return true;
}

bool ColorBuffer::IssueViewID()
{
	_resourceID[(int)ResourceID::RTV] = DirectX12::Instance().IssueViewID(HeapType::RTV);
	_resourceID[(int)ResourceID::SRV] = DirectX12::Instance().IssueViewID(HeapType::SRV);
	_resourceID[(int)ResourceID::UAV] = DirectX12::Instance().IssueViewID(HeapType::UAV);
	return true;
}
/****************************************************************************
*							CreateTexture
*************************************************************************//**
*  @fn        bool ColorBuffer::CreateTexture(int width, int height, DXGI_FORMAT format)
*  @brief     Create texture resource
*  @param[in] int width
*  @param[in] int height
*  @param[in] DXGI_FORMAT format
*  @return 　　bool
*****************************************************************************/
bool ColorBuffer::CreateTexture(int width, int height, DXGI_FORMAT format, float clearColor[4])
{
	D3D12_HEAP_PROPERTIES heapProperty = HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resourceDesc = RESOURCE_DESC::Texture2D(format, width, height, 1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	if (clearColor != nullptr)
	{
		for (int i = 0; i < 4; ++i) 
		{
			clearValue.Color[i] = clearColor[i];
			_clearColor[i] = clearColor[i];
		}
	}
	else
	{
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 1.0f;
	}

	// create resource 
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&clearValue,
			IID_PPV_ARGS(&_colorBuffer.Resource)));

	_usageState = D3D12_RESOURCE_STATE_COMMON;
	_colorBuffer.Format      = format;
	_colorBuffer.ImageSize.x = static_cast<float>(width);
	_colorBuffer.ImageSize.y = static_cast<float>(height);
	_colorBuffer.GPUHandler  = DirectX12::Instance().GetGPUResourceView
	(HeapType::SRV, _resourceID[(int)ResourceID::SRV]);
	return true;
}

/****************************************************************************
*							CreateDescriptor
*************************************************************************//**
*  @fn        bool ColorBuffer::CreateDescriptor(DXGI_FORMAT format)
*  @brief     Create descriptor (rtv, srv, uav)
*  @param[in] DXGI_FORMAT format
*  @return 　　bool
*****************************************************************************/
bool ColorBuffer::CreateDescriptor(DXGI_FORMAT format)
{

	/*-------------------------------------------------------------------
	-      Create descriptor for rtv
	---------------------------------------------------------------------*/
	D3D12_RENDER_TARGET_VIEW_DESC colorBufferDesc = {};
	colorBufferDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	colorBufferDesc.Format        = format;
	DirectX12::Instance().GetDevice()->CreateRenderTargetView(
		_colorBuffer.Resource.Get(), &colorBufferDesc,
		DirectX12::Instance().GetCPUResourceView(HeapType::RTV, _resourceID[(int)ResourceID::RTV]));

	/*-------------------------------------------------------------------
	-      Create descriptor for srv
	---------------------------------------------------------------------*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format                        = format;
	srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels           = 1;
	srvDesc.Texture2D.PlaneSlice          = 0;
	srvDesc.Texture2D.MostDetailedMip     = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	DirectX12::Instance().GetDevice()->CreateShaderResourceView(
		_colorBuffer.Resource.Get(), &srvDesc,
		DirectX12::Instance().GetCPUResourceView(HeapType::SRV, _resourceID[(int)ResourceID::SRV]));

	/*-------------------------------------------------------------------
	-      Create descriptor for uav
	---------------------------------------------------------------------*/
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension        = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice   = 0;
	uavDesc.Texture2D.PlaneSlice = 0;
	uavDesc.Format               = format;

	DirectX12::Instance().GetDevice()->CreateUnorderedAccessView(
		_colorBuffer.Resource.Get(), nullptr,
		&uavDesc, 
		DirectX12::Instance().GetCPUResourceView(HeapType::UAV, _resourceID[(int)ResourceID::UAV]));
	return true;
}
#pragma endregion Private Function
#pragma endregion Color Buffer
#pragma region RWStructuredBuffer
#pragma region Public Function
bool RWStructuredBuffer::Create(int elementByteSize, int elementCount)
{
	if (_isInitialized) { return true; }

	_elementByteSize = elementByteSize;
	_elementCount    = elementCount;

	if (!IssueViewID()) { MessageBox(NULL, L"Failed to issue view ID.", L"Warning", MB_ICONWARNING); return false; }
	if (!PrepareResource())
	{
		MessageBox(NULL, L"Failed to prepare resource buffer.", L"Warning", MB_ICONWARNING);
		return false;
	}
	if (!PrepareDescriptor())
	{
		MessageBox(NULL, L"Failed to create descriptor.", L"Warning", MB_ICONWARNING);
		return false;
	}

	_isInitialized = true;
	return true;
}

bool RWStructuredBuffer::OnResize(int elementCount)
{
	if (!_isInitialized) { return false; }
	if (elementCount != _elementCount)
	{
		_elementCount = elementCount;
		if (!PrepareResource())   { return false; }
		if (!PrepareDescriptor()) { return false; }
	}
	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE RWStructuredBuffer::GetCPUSRV() const
{
	return DirectX12::Instance().GetCPUResourceView(HeapType::SRV, _resourceID[(int)ResourceID::SRV]);
}

D3D12_CPU_DESCRIPTOR_HANDLE RWStructuredBuffer::GetCPUUAV() const
{
	return DirectX12::Instance().GetCPUResourceView(HeapType::UAV, _resourceID[(int)ResourceID::UAV]);
}

D3D12_GPU_DESCRIPTOR_HANDLE RWStructuredBuffer::GetGPUSRV() const
{
	return DirectX12::Instance().GetGPUResourceView(HeapType::SRV, _resourceID[(int)ResourceID::SRV]);
}
D3D12_GPU_DESCRIPTOR_HANDLE RWStructuredBuffer::GetGPUUAV() const
{
	return DirectX12::Instance().GetGPUResourceView(HeapType::UAV, _resourceID[(int)ResourceID::UAV]);
}
#pragma endregion Public Function
#pragma region Private Function
bool RWStructuredBuffer::IssueViewID()
{
	if (_isInitialized) { return true; }
	_resourceID[(int)ResourceID::SRV] = DirectX12::Instance().IssueViewID(HeapType::SRV);
	_resourceID[(int)ResourceID::UAV] = DirectX12::Instance().IssueViewID(HeapType::UAV);
	return true;
}

bool RWStructuredBuffer::PrepareResource()
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.CreationNodeMask     = 1;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.Type                 = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.VisibleNodeMask      = 1;

	auto buffer   = RESOURCE_DESC::Buffer((UINT64)_elementByteSize * _elementCount);
	buffer.Flags  = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&_buffer)));
	return true;
}
bool RWStructuredBuffer::PrepareDescriptor()
{

	/*-------------------------------------------------------------------
	-      Create descriptor for srv
	---------------------------------------------------------------------*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format                        = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement           = 0;
	srvDesc.Buffer.NumElements            = _elementCount;
	srvDesc.Buffer.StructureByteStride    = _elementByteSize;
	srvDesc.Buffer.Flags                  = D3D12_BUFFER_SRV_FLAG_NONE;

	DirectX12::Instance().GetDevice()->CreateShaderResourceView(
		_buffer.Get(), &srvDesc,
		DirectX12::Instance().GetCPUResourceView(HeapType::SRV, _resourceID[(int)ResourceID::SRV]));

	/*-------------------------------------------------------------------
	-      Create descriptor for uav
	---------------------------------------------------------------------*/
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension              = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements         = _elementCount;
	uavDesc.Buffer.StructureByteStride = _elementByteSize;
	uavDesc.Format                     = DXGI_FORMAT_UNKNOWN;

	DirectX12::Instance().GetDevice()->CreateUnorderedAccessView(
		_buffer.Get(), nullptr,
		&uavDesc, 
		DirectX12::Instance().GetCPUResourceView(HeapType::UAV, _resourceID[(int)ResourceID::UAV]));
	return true;
}
#pragma endregion Private Function
#pragma endregion RWStructuredBuffer
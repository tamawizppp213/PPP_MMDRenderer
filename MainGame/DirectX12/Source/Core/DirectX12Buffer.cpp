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
	ThrowIfFailed(device->CreateCommittedResource(
		&HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC::Buffer(totalByteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(_defaultBuffer.GetAddressOf())));

	/*-------------------------------------------------------------------
	-          Copy CPU memory data into our dafault buffer, 
	-          we need to create an intermediate upload heap
	---------------------------------------------------------------------*/
	ThrowIfFailed(device->CreateCommittedResource(
		&HEAP_PROPERTY(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC::Buffer(totalByteSize),
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
	commandList->ResourceBarrier(1, &BARRIER::Transition(_defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(commandList, _defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	commandList->ResourceBarrier(1, &BARRIER::Transition(_defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

DefaultBuffer::~DefaultBuffer()
{
	
}
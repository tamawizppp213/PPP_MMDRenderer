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
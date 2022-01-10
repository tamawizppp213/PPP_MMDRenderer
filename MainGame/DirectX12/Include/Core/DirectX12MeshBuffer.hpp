//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12Geometry.hpp
///             @brief  DirectX12 Structures for handling geometry
///             @author Toide Yutaro
///             @date   2020_11_23
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_MESH_BUFFER_HPP
#define DIRECTX12_MESH_BUFFER_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Core.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*							 DirectXMesh
*************************************************************************//**
*  @struct    DirectXMesh
*  @brief     DirectXMesh struct
*****************************************************************************/
struct MeshBuffer
{
public:
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	BlobComPtr     VertexBufferCPU      = nullptr;
	BlobComPtr     IndexBufferCPU       = nullptr;
	ResourceComPtr VertexBufferGPU      = nullptr;
	ResourceComPtr IndexBufferGPU       = nullptr;
	ResourceComPtr VertexBufferUploader = nullptr;
	ResourceComPtr IndexBufferUploader  = nullptr;

	UINT IndexCount = 0;
	UINT StartIndexLocation   = 0;
	INT  BaseVertexLocation   = 0;
	UINT VertexByteStride     = 0;
	UINT VertexBufferByteSize = 0;
	UINT IndexBufferByteSize  = 0;
	DXGI_FORMAT IndexFormat   = DXGI_FORMAT_R16_UINT;

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	inline D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes  = VertexByteStride;
		vbv.SizeInBytes    = VertexBufferByteSize;

		return vbv;
	}

	inline D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format         = IndexFormat;
		ibv.SizeInBytes    = IndexBufferByteSize;

		return ibv;
	}

	inline void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader  = nullptr;
	}

	inline void Dispose()
	{
		if (VertexBufferCPU) { VertexBufferCPU = nullptr; }
		if (IndexBufferCPU)  { IndexBufferCPU  = nullptr; }
		if (VertexBufferGPU) { VertexBufferGPU = nullptr; }
		if (IndexBufferGPU)  { IndexBufferGPU  = nullptr; }
		if (VertexBufferUploader) { VertexBufferUploader = nullptr; }
		if (IndexBufferUploader) { IndexBufferUploader   = nullptr; }
	}
	/****************************************************************************
	**                Constructor
	*****************************************************************************/
	MeshBuffer() = default;
	MeshBuffer(const MeshBuffer&)            = default;
	MeshBuffer& operator=(const MeshBuffer&) = default;
	MeshBuffer(MeshBuffer&&)                 = default;
	MeshBuffer& operator=(MeshBuffer&&)      = default;
	~MeshBuffer() { }
private:
};


#endif
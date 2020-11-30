//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12Geometry.hpp
///             @brief  DirectX12 Structures for handling geometry
///             @author Toide Yutaro
///             @date   2020_11_23
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_GEOMETRY_HPP
#define DIRECTX12_GEOMETRY_HPP
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Core.hpp"
#include <DirectXCollision.h>
#include <dxgi1_6.h>
#include <string>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*							 SubMeshGeometry
*************************************************************************//**
*  @struct    SubMeshGeometry
*  @brief     Geomety SubMesh struct
*****************************************************************************/
struct SubMeshGeometry
{
public:
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	UINT IndexCount         = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation  = 0;

	DirectX::BoundingBox Bounds;

	/****************************************************************************
	**                Constructor
	*****************************************************************************/
	SubMeshGeometry()                                  = default;
	SubMeshGeometry(const SubMeshGeometry&)            = default;
	SubMeshGeometry& operator=(const SubMeshGeometry&) = default;
	SubMeshGeometry(SubMeshGeometry&&)                 = default;
	SubMeshGeometry& operator=(SubMeshGeometry&&)      = default;
private:
};
/****************************************************************************
*							 MeshGeometry
*************************************************************************//**
*  @struct    MeshGeometry
*  @brief     Geomety Mesh struct 
*  @details   A MeshGeometry can store multiple geometries in one vertex/index buffer.   
*****************************************************************************/
struct MeshGeometry
{
public:
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::string Name;

	BlobComPtr     VertexBufferCPU      = nullptr;
	BlobComPtr     IndexBufferCPU       = nullptr;
	ResourceComPtr VertexBufferGPU      = nullptr;
	ResourceComPtr IndexBufferGPU       = nullptr;
	ResourceComPtr VertexBufferUploader = nullptr;
	ResourceComPtr IndexBufferUploader  = nullptr;

	UINT VertexByteStride       = 0;
	UINT VertexBufferByteSize = 0;
	UINT IndexBufferByteSize    = 0;
	DXGI_FORMAT IndexFormat     = DXGI_FORMAT_R16_UINT;

	//  Use this container to define the Submesh geometries so we can draw the Submeshes individually.
	std::unordered_map<std::string, SubMeshGeometry> DrawArgs;

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
	/****************************************************************************
	**                Constructor
	*****************************************************************************/
	MeshGeometry()                               = default;
	MeshGeometry(const MeshGeometry&)            = default;
	MeshGeometry& operator=(const MeshGeometry&) = default;
	MeshGeometry(MeshGeometry&&)                 = default;
	MeshGeometry& operator=(MeshGeometry&&)      = default;
private:
};


#endif